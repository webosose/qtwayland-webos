// Copyright (c) 2013-2021 LG Electronics, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// SPDX-License-Identifier: Apache-2.0

#include "waylandinputcontext.h"
#include <qpa/qplatformnativeinterface.h>
#include <qpa/qwindowsysteminterface.h>
#include <QtGui/QGuiApplication>
#include <QtCore/QCoreApplication>

#include <QTextCharFormat>
#include <QInputMethodEvent>
#include "keysymhelper.h"

#include <QDebug>
#include <QRectF>

#include "webosinputpanellocator.h"
#include "securecoding.h"

const struct wl_registry_listener WaylandInputContext::registryListener = {
    WaylandInputContext::registryGlobalAdded,
    WaylandInputContext::registryGlobalRemoved
};

const struct text_model_listener WaylandInputContext::textModelListener = {
    WaylandInputContext::textModelCommitString,
    WaylandInputContext::textModelPreEditString,
    WaylandInputContext::textModelDeleteSurroundingText,
    WaylandInputContext::textModelCursorPosition,
    WaylandInputContext::textModelPreEditStyling,
    WaylandInputContext::textModelPreEditCursor,
    WaylandInputContext::textModelModifiersMap,
    WaylandInputContext::textModelKeySym,
    WaylandInputContext::textModelEnter,
    WaylandInputContext::textModelLeave,
    WaylandInputContext::textModelInputPanelState,
    WaylandInputContext::textModelInputPanelRect
};

static uint32_t contentHintFromQtHints(Qt::InputMethodHints hints)
{
    // Qt assumes that these are always desired. They will be masked out below
    // if specified
    uint32_t wlHint = (TEXT_MODEL_CONTENT_HINT_AUTO_COMPLETION | TEXT_MODEL_CONTENT_HINT_AUTO_CAPITALIZATION);

    if (hints & Qt::ImhHiddenText) { wlHint |= TEXT_MODEL_CONTENT_HINT_PASSWORD; }
    if (hints & Qt::ImhSensitiveData) { wlHint |= TEXT_MODEL_CONTENT_HINT_SENSITIVE_DATA; }
    if (hints & Qt::ImhNoAutoUppercase) { wlHint &= ~TEXT_MODEL_CONTENT_HINT_AUTO_CAPITALIZATION; }
    if (hints & Qt::ImhPreferUppercase) { wlHint |= TEXT_MODEL_CONTENT_HINT_UPPERCASE; }
    if (hints & Qt::ImhPreferLowercase) { wlHint |= TEXT_MODEL_CONTENT_HINT_LOWERCASE; }
    if (hints & Qt::ImhNoPredictiveText) { wlHint &= ~TEXT_MODEL_CONTENT_HINT_AUTO_COMPLETION; }
    if (hints & Qt::ImhPreferLatin) { wlHint |= TEXT_MODEL_CONTENT_HINT_LATIN; }

    return wlHint;
}

static uint32_t contentPurposeFromQtHints(Qt::InputMethodHints hints)
{
    Qt::InputMethodHints exclusiveHints = hints & Qt::ImhExclusiveInputMask;
    uint32_t wlPurpose = 0;
    switch (exclusiveHints) {
        case Qt::ImhDigitsOnly: wlPurpose |= TEXT_MODEL_CONTENT_PURPOSE_DIGITS; break;
        case Qt::ImhFormattedNumbersOnly: wlPurpose |= TEXT_MODEL_CONTENT_PURPOSE_NUMBER; break;
        case Qt::ImhDialableCharactersOnly: wlPurpose |= TEXT_MODEL_CONTENT_PURPOSE_PHONE; break;
        case Qt::ImhEmailCharactersOnly: wlPurpose |= TEXT_MODEL_CONTENT_PURPOSE_EMAIL; break;
        case Qt::ImhUrlCharactersOnly: wlPurpose |= TEXT_MODEL_CONTENT_PURPOSE_URL; break;
    }

    if (hints & Qt::ImhPreferNumbers) { wlPurpose |= TEXT_MODEL_CONTENT_PURPOSE_NUMBER; }
    if (hints & Qt::ImhDate) { wlPurpose |= TEXT_MODEL_CONTENT_PURPOSE_DATE; }
    if (hints & Qt::ImhTime) { wlPurpose |= TEXT_MODEL_CONTENT_PURPOSE_TIME; }
    if (hints & Qt::ImhHiddenText) { wlPurpose |= TEXT_MODEL_CONTENT_PURPOSE_PASSWORD; }

    return wlPurpose;
}

static uint32_t enterKeyTypeFromQtEnterKeyTypes(Qt::EnterKeyType enterType)
{
    uint32_t wlEnterType = TEXT_MODEL_ENTER_KEY_TYPE_DEFAULT;
    switch (enterType) {
        case Qt::EnterKeyDefault: wlEnterType = TEXT_MODEL_ENTER_KEY_TYPE_DEFAULT; break;
        case Qt::EnterKeyReturn: wlEnterType = TEXT_MODEL_ENTER_KEY_TYPE_RETURN; break;
        case Qt::EnterKeyDone: wlEnterType = TEXT_MODEL_ENTER_KEY_TYPE_DONE; break;
        case Qt::EnterKeyGo: wlEnterType = TEXT_MODEL_ENTER_KEY_TYPE_GO; break;
        case Qt::EnterKeySend: wlEnterType = TEXT_MODEL_ENTER_KEY_TYPE_SEND; break;
        case Qt::EnterKeySearch: wlEnterType = TEXT_MODEL_ENTER_KEY_TYPE_SEARCH; break;
        case Qt::EnterKeyNext: wlEnterType = TEXT_MODEL_ENTER_KEY_TYPE_NEXT; break;
        case Qt::EnterKeyPrevious: wlEnterType = TEXT_MODEL_ENTER_KEY_TYPE_PREVIOUS; break;
    }

    return wlEnterType;
}

static QTextCharFormat qtStylingFrom(uint32_t style)
{
    QTextCharFormat format;
    switch (style) {
        case TEXT_MODEL_PREEDIT_STYLE_DEFAULT: break;
        case TEXT_MODEL_PREEDIT_STYLE_ACTIVE: break;
        case TEXT_MODEL_PREEDIT_STYLE_INACTIVE: break;
        case TEXT_MODEL_PREEDIT_STYLE_HIGHLIGHT: format.setBackground(QBrush(QColor(198, 176, 186))); break;
        case TEXT_MODEL_PREEDIT_STYLE_UNDERLINE: break;
        case TEXT_MODEL_PREEDIT_STYLE_SELECTION: break;
        case TEXT_MODEL_PREEDIT_STYLE_INCORRECT: break;
    }
    return format;
}

static int serial = 0;

WaylandInputContext::WaylandInputContext()
    : m_focusObject(0)
    , m_display(0)
    , m_registry(0)
    , m_seat(0)
    , m_textModelFactory(0)
    , m_currentTextModel(0)
    , m_pendingQueries(Qt::ImEnabled)
    , m_isQueryPending(false)
    , m_isCleanupPending(false)
    , m_isActivationPending(false)
    , m_modelActivated(false)
    , m_inputPanelState(InputPanelUnknownState)
{
    ensureWaylandConnection();

    connect(WebOSInputPanelLocator::instance(), &WebOSInputPanelLocator::inputPanelRectChanged,
        this, &WaylandInputContext::updateInputPanelRect);
}

WaylandInputContext::~WaylandInputContext()
{
    cleanup();

    if (m_registry) {
        wl_registry_destroy(m_registry);
    }
}

bool WaylandInputContext::isValid() const
{
#ifdef WAYLAND_INPUT_CONTEXT_DEBUG
    qDebug() << "true";
#endif
    return true;
}

void WaylandInputContext::reset()
{
#ifdef WAYLAND_INPUT_CONTEXT_DEBUG
    qDebug() << __PRETTY_FUNCTION__;
#endif
    commitAndReset();
}

void WaylandInputContext::commit()
{
#ifdef WAYLAND_INPUT_CONTEXT_DEBUG
    qDebug() << __PRETTY_FUNCTION__ << m_currentTextModel;
#endif
    if (!m_currentTextModel) {
        return;
    }

    QInputMethodEvent event;
    event.setCommitString(m_preEditData.preEdit);
    resetPreEditData();
    QGuiApplication::sendEvent(m_focusObject, &event);
    text_model_commit(m_currentTextModel);
    text_model_reset(m_currentTextModel, serial);
}

void WaylandInputContext::update(Qt::InputMethodQueries queries)
{
#ifdef WAYLAND_INPUT_CONTEXT_DEBUG
    qDebug() << __PRETTY_FUNCTION__ << queries;
#endif

    if (!m_focusObject) {
        return;
    }

    m_pendingQueries |= queries;
    if (!m_modelActivated) {
        m_pendingQueries |= queries;
        m_isQueryPending = true;
        return;
    }

    Q_ASSERT(m_currentTextModel);
    queries |= m_pendingQueries;
    QInputMethodQueryEvent query(queries);
    QCoreApplication::sendEvent(m_focusObject, &query);

    if (queries & Qt::ImHints) {
        Qt::InputMethodHints hints = Qt::InputMethodHints(query.value(Qt::ImHints).toUInt());
        text_model_set_content_type(m_currentTextModel, contentHintFromQtHints(hints), contentPurposeFromQtHints(hints));
    }

    if (queries & Qt::ImEnterKeyType) {
        Qt::EnterKeyType enterKeyType = Qt::EnterKeyType(query.value(Qt::ImEnterKeyType).toUInt());
        text_model_set_enter_key_type(m_currentTextModel, enterKeyTypeFromQtEnterKeyTypes(enterKeyType));
    }

    if (queries & Qt::ImMaximumTextLength) {
        text_model_set_max_text_length(m_currentTextModel, query.value(Qt::ImMaximumTextLength).toUInt());
    }

    if (queries & Qt::ImPlatformData) {
        text_model_set_platform_data(m_currentTextModel, query.value(Qt::ImPlatformData).toString().toUtf8().data());
    }

    if (queries & Qt::ImSurroundingText ||
        queries & Qt::ImAnchorPosition ||
        queries & Qt::ImCursorPosition) {
        updateSurroundingText(query.value(Qt::ImSurroundingText), query.value(Qt::ImCursorPosition), query.value(Qt::ImAnchorPosition));
    }

    m_pendingQueries = Qt::ImEnabled;
    m_isQueryPending = false;
}

void WaylandInputContext::resetPreEditData()
{
    m_preEditData.formats.clear();
    m_preEditData.preEdit.clear();
    m_preEditData.cursor = 0;
}

void WaylandInputContext::commitAndReset(bool keepCursorPosition)
{
    bool inPreEdit = !m_preEditData.preEdit.isEmpty();
#ifdef WAYLAND_INPUT_CONTEXT_DEBUG
    qDebug() << __PRETTY_FUNCTION__ << inPreEdit << keepCursorPosition;
#endif
    if (inPreEdit && inputMethodAccepted()) {
        QList<QInputMethodEvent::Attribute> attrs;
        if (keepCursorPosition) {
            // Set attribute to move the cursor back to the original position
            // after committing the preedit data.
            QInputMethodQueryEvent query(Qt::ImCursorPosition);
            QCoreApplication::sendEvent(m_focusObject, &query);
            attrs.append(QInputMethodEvent::Attribute(QInputMethodEvent::Selection, query.value(Qt::ImCursorPosition).toInt(), 0, QVariant()));
        }
        QInputMethodEvent event(QString(""), attrs);
        event.setCommitString(m_preEditData.preEdit);

        // Commit the preedit data
        QGuiApplication::sendEvent(m_focusObject, &event);

        // Reset
        resetPreEditData();
        if (m_currentTextModel) {
            text_model_reset(m_currentTextModel, serial);
        }
    }
}

void WaylandInputContext::updateSurroundingText(const QVariant& text, const QVariant& cursor, const QVariant& anchor)
{
    Q_ASSERT(m_currentTextModel);
#ifdef WAYLAND_INPUT_CONTEXT_DEBUG
    qDebug() << __PRETTY_FUNCTION__ << m_currentTextModel << text << cursor << anchor;
#endif
    /* Retricts length of surround text to 200 characters. This implies the length will not exceed 800 bytes.
       App will be crashed when text is longer than 999 bytes. Usually wayland can carry parameters which is less than 1024 bytes. */
    /* Surrounding text and cursor position is used for word prediction in IME.
       If surrounding_text length exceeds limit, only first 200 of text is sent and cursor beyond limit makes the event values inconsistent.
       Crop text based on cursor position, and adjust cursor position based on that string.
       Anchor position still may be inconsistent, but there's no use case for now. */
    //TODO : remove limit for surrounding text because now it seems there is no limit for size of wayland buffer.
    static const int SURROUNDING_TEXT_MAX = 200;
    QString surroundingText;
    int cursorPosition = cursor.toInt();
    int anchorPosition = anchor.toInt();

    if (SURROUNDING_TEXT_MAX < cursorPosition) {
        int shifted = cursorPosition - SURROUNDING_TEXT_MAX;
        surroundingText = text.toString().left(cursorPosition).right(SURROUNDING_TEXT_MAX);

        if (anchorPosition < shifted)
            anchorPosition = 0;
        else if (anchorPosition < cursorPosition)
            anchorPosition -= shifted;
        else
            anchorPosition = SURROUNDING_TEXT_MAX; //Nothing will be selected.

        cursorPosition = SURROUNDING_TEXT_MAX;
    } else {
        surroundingText = text.toString().left(SURROUNDING_TEXT_MAX);
        if (anchorPosition > SURROUNDING_TEXT_MAX)
            anchorPosition = SURROUNDING_TEXT_MAX;
    }

    text_model_set_surrounding_text(m_currentTextModel, surroundingText.toUtf8().data(), cursorPosition, anchorPosition);
}

void WaylandInputContext::invokeAction(QInputMethod::Action action, int cursorPosition)
{
#ifdef WAYLAND_INPUT_CONTEXT_DEBUG
    qDebug() << __PRETTY_FUNCTION__ << action << cursorPosition;
#endif
    if (!inputMethodAccepted() || !m_currentTextModel)
            return;

    if (action == QInputMethod::Click) {
        if (cursorPosition <= 0 || cursorPosition >= m_preEditData.preEdit.length()) {
            commitAndReset((cursorPosition == 0));
            return;
        }
        text_model_invoke_action(m_currentTextModel, 0, cursorPosition);
    } else {
        QPlatformInputContext::invokeAction(action, cursorPosition);
    }
}

QRectF WaylandInputContext::keyboardRect() const
{
#ifdef WAYLAND_INPUT_CONTEXT_DEBUG
    qDebug() << m_keyboardRect;
#endif
    return m_keyboardRect;
}

bool WaylandInputContext::isAnimating() const
{
#ifdef WAYLAND_INPUT_CONTEXT_DEBUG
    qDebug() << "false";
#endif
    return false;
}

void WaylandInputContext::updateInputPanelRect(const QObject* targetObj,
                                             const QRect& rect)
{
    if (!m_currentTextModel)
        return;

    // Check if targetObj is focused and rect differs from the previous one
    if (targetObj == m_focusObject && m_inputPanelRectRequested != rect) {
        text_model_set_input_panel_rect(m_currentTextModel,
                                        rect.x(), rect.y(),
                                        rect.width(), rect.height());
        m_inputPanelRectRequested = rect;
#ifdef WAYLAND_INPUT_CONTEXT_DEBUG
        qDebug() << "inputPanelRect" << inputPanelRect;
#endif
    }
}

void WaylandInputContext::resetInputPanelRect(const QObject* targetObj)
{
    if (!m_currentTextModel)
        return;

    if (targetObj == m_focusObject) {
        text_model_reset_input_panel_rect(m_currentTextModel);
        m_inputPanelRectRequested.setRect(0, 0, 0, 0);
#ifdef WAYLAND_INPUT_CONTEXT_DEBUG
        qDebug() << "reset inputPanelRect";
#endif
    }
}

void WaylandInputContext::showInputPanel()
{
#ifdef WAYLAND_INPUT_CONTEXT_DEBUG
    qDebug() << "currentTextModel" << m_currentTextModel << "accepted " << inputMethodAccepted();
#endif
    if (!m_seat || !inputMethodAccepted())
        return;

    if (m_isCleanupPending)
        cleanup();

    if (!m_modelActivated) {
        if (!m_isActivationPending) {
            QPlatformNativeInterface *nativeInterface = QGuiApplication::platformNativeInterface();
            if (!nativeInterface) {
                qWarning() << "No nativeInterface available";
                return;
            }
            wl_surface* surface = (wl_surface*) nativeInterface->nativeResourceForWindow("surface", QGuiApplication::focusWindow());
            if (!surface) {
                qWarning() << "Surface for focus window is already destroyed.";
                return;
            }

            m_currentTextModel = text_model_factory_create_text_model(m_textModelFactory);
            text_model_add_listener(m_currentTextModel, &textModelListener, this);

            updateInputPanelRect(m_focusObject, WebOSInputPanelLocator::instance()->inputPanelRect(m_focusObject));

            text_model_activate(m_currentTextModel, serial++, m_seat, surface);
            m_isActivationPending = true;
        } else {
            qWarning() << "Text model activation has been requested already.";
            updateInputPanelRect(m_focusObject, WebOSInputPanelLocator::instance()->inputPanelRect(m_focusObject));
        }
    } else {
        updateInputPanelRect(m_focusObject, WebOSInputPanelLocator::instance()->inputPanelRect(m_focusObject));

        // This will execute pending queries also.
        update(Qt::ImHints | Qt::ImSurroundingText | Qt::ImAnchorPosition | Qt::ImCursorPosition | Qt::ImEnterKeyType | Qt::ImMaximumTextLength | Qt::ImPlatformData);
    }
}

void WaylandInputContext::hideInputPanel()
{
#ifdef WAYLAND_INPUT_CONTEXT_DEBUG
    qDebug() << "currentTextModel" << m_currentTextModel << "seat" << m_seat;
#endif
    if (!m_currentTextModel)
        return;

    // Finish editing surrounding text before input pannel hiding
    commitAndReset();

    cleanup();
    m_inputPanelState = InputPanelUnknownState;
    emitInputPanelVisibleChanged();
}

bool WaylandInputContext::isInputPanelVisible() const
{
#ifdef WAYLAND_INPUT_CONTEXT_DEBUG
    qDebug() << m_currentTextModel;
#endif
    return m_currentTextModel && (m_inputPanelState == InputPanelShown || m_inputPanelState == InputPanelShowing);
}

QLocale WaylandInputContext::locale() const
{
#ifdef WAYLAND_INPUT_CONTEXT_DEBUG
    qDebug() << "return system locale";
#endif
    return QLocale::system();
}

Qt::LayoutDirection WaylandInputContext::inputDirection() const
{
#ifdef WAYLAND_INPUT_CONTEXT_DEBUG
    qDebug() << "Qt::LeftToRight";
#endif
    return Qt::LeftToRight;
}

void WaylandInputContext::setFocusObject(QObject *object)
{
#ifdef WAYLAND_INPUT_CONTEXT_DEBUG
    qDebug() << object << m_currentTextModel << inputMethodAccepted();
#endif
    if (m_focusObject == object)
        return;

    if (m_focusObject)
        disconnect(m_focusObject, SIGNAL(destroyed(QObject *)), this, SLOT(focusObjectDestroyed(QObject *)));

    m_focusObject = object;

    if (m_focusObject)
        connect(m_focusObject, SIGNAL(destroyed(QObject *)), this, SLOT(focusObjectDestroyed(QObject *)));

    if (!m_textModelFactory || !m_display)
        return;

    if (inputMethodAccepted() && qGuiApp->focusWindow()) {
        // Reset inputPanelRect as the focused object changes
        resetInputPanelRect(m_focusObject);
        showInputPanel();
    } else {
        hideInputPanel();
    }
}

void WaylandInputContext::focusObjectDestroyed(QObject *object)
{
    if (m_focusObject == object)
        m_focusObject = 0;
}

void WaylandInputContext::ensureWaylandConnection()
{
    if (m_display) {
        return;
    }

    QPlatformNativeInterface *nativeInterface = QGuiApplication::platformNativeInterface();
    if (nativeInterface) {
        m_display = static_cast<wl_display*>(nativeInterface->nativeResourceForIntegration("display"));
    }
    if (!m_display) {
        qWarning("No wayland display available");
        return;
    }
#ifdef WAYLAND_INPUT_CONTEXT_DEBUG
    qDebug() << m_display;
#endif
    m_registry = wl_display_get_registry(m_display);
    wl_registry_add_listener(m_registry, &registryListener, this);
}

// Wayland Protocol methods (static)
void WaylandInputContext::registryGlobalAdded(void *data,
                                           struct wl_registry *registry,
                                           uint32_t id,
                                           const char *interface,
                                           uint32_t version)
{
    Q_UNUSED(version);
    Q_UNUSED(registry);

    // For convenience...
    QByteArray interfaceName(interface);
    WaylandInputContext* that = static_cast<WaylandInputContext*>(data);
    if (interfaceName == "text_model_factory") {
        that->m_textModelFactory = static_cast<text_model_factory *>(wl_registry_bind(that->m_registry, id, &text_model_factory_interface, 1));
    } else if (interfaceName == "wl_seat") {
        that->m_seat = static_cast<wl_seat*>(wl_registry_bind(that->m_registry, id, &wl_seat_interface, 1));
    }
}

void WaylandInputContext::registryGlobalRemoved(void *data,
                                             wl_registry *registry,
                                             uint32_t name)
{
    Q_UNUSED(data);
    Q_UNUSED(registry);
    Q_UNUSED(name);
}

void WaylandInputContext::textModelCommitString(void *data, struct text_model *text_model, uint32_t serial, const char *text)
{
    Q_UNUSED(text_model);
    Q_UNUSED(serial);

#ifdef WAYLAND_INPUT_CONTEXT_DEBUG
    qDebug() << __PRETTY_FUNCTION__ << QString(text);
#endif

    WaylandInputContext* that = static_cast<WaylandInputContext*>(data);
    QInputMethodEvent *event = new QInputMethodEvent();
    event->setCommitString(QString(text), 0, 0);
    QCoreApplication::postEvent(that->m_focusObject, event);
    that->resetPreEditData();
}

void WaylandInputContext::textModelPreEditString(void *data, struct text_model *text_model, uint32_t serial, const char *text, const char* commit)
{
    Q_UNUSED(text_model);
    Q_UNUSED(serial);
    Q_UNUSED(commit);
#ifdef WAYLAND_INPUT_CONTEXT_DEBUG
    qDebug() << __PRETTY_FUNCTION__ << QString(text) << QString(commit);
#endif
    WaylandInputContext* that = static_cast<WaylandInputContext*>(data);

    that->m_preEditData.preEdit = QString(text);
    that->m_preEditData.formats << QInputMethodEvent::Attribute(QInputMethodEvent::Cursor, that->m_preEditData.preEdit.length(), 1, QVariant());
    QInputMethodEvent *event = new QInputMethodEvent(that->m_preEditData.preEdit, that->m_preEditData.formats);
//    resetPreEditData();
    QCoreApplication::postEvent(that->m_focusObject, event);
}

void WaylandInputContext::textModelDeleteSurroundingText(void *data, struct text_model *text_model, uint32_t serial, int32_t index, uint32_t length)
{
    Q_UNUSED(text_model);
    Q_UNUSED(serial);
#ifdef WAYLAND_INPUT_CONTEXT_DEBUG
    qDebug() << __PRETTY_FUNCTION__ << index << length;
#endif
    WaylandInputContext* that = static_cast<WaylandInputContext*>(data);
    QList<QInputMethodEvent::Attribute> attributes;
    QInputMethodEvent *event = new QInputMethodEvent(QString(""), attributes);

    if (length == UINT_MAX) {
        QInputMethodQueryEvent query(Qt::ImSurroundingText | Qt::ImCursorPosition);
        QCoreApplication::sendEvent(that->m_focusObject, &query);
        event->setCommitString("", -query.value(Qt::ImCursorPosition).toInt(), query.value(Qt::ImSurroundingText).toString().length());
    } else {
        event->setCommitString("", index, length);
    }

    QCoreApplication::postEvent(that->m_focusObject, event);
}

void WaylandInputContext::textModelCursorPosition(void* data, struct text_model *text_model, uint32_t serial, int32_t index, int32_t anchor)
{
    Q_UNUSED(data);
    Q_UNUSED(text_model);
    Q_UNUSED(serial);
    Q_UNUSED(index);
    Q_UNUSED(anchor);
#ifdef WAYLAND_INPUT_CONTEXT_DEBUG
    qDebug() << index << anchor;
#endif
}

void WaylandInputContext::textModelPreEditStyling(void *data, struct text_model *text_model, uint32_t serial, uint32_t index, uint32_t length, uint32_t style)
{
    Q_UNUSED(data);
    Q_UNUSED(text_model);
    Q_UNUSED(serial);
    Q_UNUSED(index);
    Q_UNUSED(length);
    Q_UNUSED(style);
#ifdef WAYLAND_INPUT_CONTEXT_DEBUG
    qDebug() << __PRETTY_FUNCTION__ << index << length << style;
#endif
    WaylandInputContext* that = static_cast<WaylandInputContext*>(data);
    that->m_preEditData.formats << QInputMethodEvent::Attribute(QInputMethodEvent::TextFormat, uint2int(index), uint2int(length), qtStylingFrom(style));
}

void WaylandInputContext::textModelPreEditCursor(void *data, struct text_model *text_model, uint32_t serial, int32_t index)
{
    Q_UNUSED(data);
    Q_UNUSED(text_model);
    Q_UNUSED(serial);
    WaylandInputContext* that = static_cast<WaylandInputContext*>(data);
#ifdef WAYLAND_INPUT_CONTEXT_DEBUG
    qDebug() << __PRETTY_FUNCTION__ << index;
#endif
    that->m_preEditData.cursor = index;
}

void WaylandInputContext::textModelModifiersMap(void *data, struct text_model *text_model, struct wl_array *map)
{
    Q_UNUSED(text_model);
#ifdef WAYLAND_INPUT_CONTEXT_DEBUG
    qDebug() << __PRETTY_FUNCTION__;
#endif
    WaylandInputContext* that = static_cast<WaylandInputContext*>(data);
    that->updateModifiersMap(map);
}

void WaylandInputContext::updateModifiersMap(struct wl_array *map)
{
    m_modifiersMap.applyWaylandModifiersMap(map);
}

Qt::KeyboardModifiers WaylandInputContext::convertNativeModifiersToQt(uint32_t modifiers)
{
    return m_modifiersMap.convertNativeModifiersToQt(modifiers);
}

void WaylandInputContext::textModelKeySym(void *data, struct text_model *text_model, uint32_t serial, uint32_t time, uint32_t sym, uint32_t state, uint32_t modifiers)
{
    Q_UNUSED(text_model);
    Q_UNUSED(serial);
    QEvent::Type type = (state != 0) ? QEvent::KeyPress : QEvent::KeyRelease;
    int qtKey = xkbKeyToQtKey(sym);

    /* TODO: Need more centralized control on keymaps.
     *
     * Cause our new keymap system returns qt-code as xkbsym on all key defined in
     * starfishkeyboard_defaultmap_p.h of QPA,
     * we can't find entry on translate table of xkb to qt-key.
     * So for this LG remote key we can use xkb_sym as qtkey. This is same method with
     * qtwayland's client plugin;platforms/wayland_common_webos/qwaylandinputdevice.cpp;
     */
    if (qtKey == Qt::Key_unknown) {
        qtKey = uint2int(sym);
    }

    WaylandInputContext* that = static_cast<WaylandInputContext*>(data);

    Qt::KeyboardModifiers qtModifiers =
        that->convertNativeModifiersToQt(modifiers);
    if (isKeypadKey(sym))
        qtModifiers |= Qt::KeypadModifier;

#ifdef WAYLAND_INPUT_CONTEXT_DEBUG
    qDebug() << __PRETTY_FUNCTION__ << "qt key" << qtKey << "modifier" << qtModifiers;
#endif

    QString str;

    switch (qtKey) {
    case Qt::Key_Enter:
    case Qt::Key_Return:
        // Temporary way to map the keys until xkbcommon is in use [GF-4360]
        str = QString("\r");
        break;
    case Qt::Key_Left:
    case Qt::Key_Right:
        // Make sure the preedit string committed before moving the cursor
        that->commitAndReset(qtKey == Qt::Key_Right);
        break;
    }

    QWindowSystemInterface::handleExtendedKeyEvent(qGuiApp->focusWindow(),
                                                   time, type, qtKey,
                                                   qtModifiers,
                                                   qtKey, 0, 0,
                                                   str);
}

void WaylandInputContext::textModelEnter(void *data, struct text_model *text_model, struct wl_surface *surface)
{
    WaylandInputContext* that = static_cast<WaylandInputContext*>(data);
    that->m_modelActivated = true;
    that->m_isActivationPending = false;

    that->m_inputPanelState = InputPanelShowing;
    that->emitInputPanelVisibleChanged();

    // This will execute pending queries also.
    that->update(Qt::ImHints | Qt::ImSurroundingText | Qt::ImAnchorPosition | Qt::ImCursorPosition | Qt::ImEnterKeyType | Qt::ImMaximumTextLength | Qt::ImPlatformData);

    Q_UNUSED(text_model);
    Q_UNUSED(surface);
}

void WaylandInputContext::textModelLeave(void *data, struct text_model *text_model)
{
    Q_UNUSED(text_model);
    WaylandInputContext* that = static_cast<WaylandInputContext*>(data);
    that->hideInputPanel();
}

void WaylandInputContext::textModelInputPanelState(void *data, struct text_model *text_model, uint32_t state)
{
    Q_UNUSED(text_model);
    WaylandInputContext* that = static_cast<WaylandInputContext*>(data);

    if (that->m_inputPanelState != state) {
        // FIXME
        // 1. There is no way to show already activated model.
        //    Then we need to destroy old model when it was hidden by server-side.
        // 2. There exist a timing issue when switching inputs quickly
        if (state == InputPanelHidden && that->m_inputPanelState != InputPanelShowing)
            that->m_isCleanupPending = true;
        that->m_inputPanelState = (InputPanelState)state;
        that->emitInputPanelVisibleChanged();
    }
}

void WaylandInputContext::textModelInputPanelRect(void *data, struct text_model *text_model, int32_t x, int32_t y, uint32_t width, uint32_t height)
{
    Q_UNUSED(text_model);
    WaylandInputContext* that = static_cast<WaylandInputContext*>(data);
    QRectF newRect(x, y, width, height);
    if (that->m_keyboardRect != newRect) {
        that->m_keyboardRect = newRect;
        qDebug() << "emitKeyboardRectChanged" << newRect;
        that->emitKeyboardRectChanged();
    }
}
void WaylandInputContext::cleanup()
{
    if (m_currentTextModel) {
        text_model_deactivate(m_currentTextModel, m_seat);
        text_model_destroy(m_currentTextModel);
        m_currentTextModel = NULL;
    }

    m_pendingQueries = Qt::ImEnabled;
    m_isQueryPending = false;
    m_isCleanupPending = false;
    m_isActivationPending = false;
    m_modelActivated = false;
    m_inputPanelRectRequested.setRect(0, 0, 0, 0);
}
