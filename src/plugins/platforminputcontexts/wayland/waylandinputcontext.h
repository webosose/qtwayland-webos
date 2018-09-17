// Copyright (c) 2013-2018 LG Electronics, Inc.
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

#ifndef WAYLANDINPUTCONTEXT_H
#define WAYLANDINPUTCONTEXT_H

#include <qpa/qplatforminputcontext.h>

#include <QInputMethodEvent>
#include <wayland-client.h>
#include <wayland-text-client-protocol.h>

#include <xkbcommon/xkbcommon.h>
#include "keysymhelper.h"

// Encapsulates data that is valid within one preedit session
class PreEditData {

public:
    PreEditData() {}

    QList<QInputMethodEvent::Attribute> formats;
    QString preEdit;
    int32_t cursor;
};


class WaylandInputContext : public QPlatformInputContext
{
    Q_OBJECT

public:
    enum InputPanelState {
        InputPanelUnknownState = 0xffffffff,
        InputPanelHidden = 0,
        InputPanelShown = 1,
        InputPanelShowing = 2
    };

public:
    WaylandInputContext();
    virtual ~WaylandInputContext();

    virtual bool isValid() const;

    virtual void reset();
    virtual void commit();
    virtual void update(Qt::InputMethodQueries);
    virtual void invokeAction(QInputMethod::Action, int cursorPosition);
    virtual QRectF keyboardRect() const;

    virtual bool isAnimating() const;

    virtual void showInputPanel();
    virtual void hideInputPanel();
    virtual bool isInputPanelVisible() const;

    virtual QLocale locale() const;
    virtual Qt::LayoutDirection inputDirection() const;

    virtual void setFocusObject(QObject *object);

    // Wayland protocol methods
    static void registryGlobalAdded(void *data, struct wl_registry *registry, uint32_t id, const char *interface, uint32_t version);
    static void registryGlobalRemoved(void *data, wl_registry *registry, uint32_t name);

    static void textModelCommitString(void *data, struct text_model *text_model, uint32_t serial, const char *text);
    static void textModelPreEditString(void *data, struct text_model *text_model, uint32_t serial, const char *text, const char* commit);
    static void textModelDeleteSurroundingText(void *data, struct text_model *text_model, uint32_t serial, int32_t index, uint32_t length);
    static void textModelCursorPosition(void* data, struct text_model *text_model, uint32_t serial, int32_t index, int32_t anchor);
    static void textModelPreEditStyling(void *data, struct text_model *text_model, uint32_t serial, uint32_t index, uint32_t length, uint32_t style);
    static void textModelPreEditCursor(void *data, struct text_model *text_model, uint32_t serial, int32_t index);
    static void textModelModifiersMap(void *data, struct text_model *text_model, struct wl_array *map);
    static void textModelKeySym(void *data, struct text_model *text_model, uint32_t serial, uint32_t time, uint32_t sym, uint32_t state, uint32_t modifiers);
    static void textModelEnter(void *data, struct text_model *text_model, struct wl_surface *surface);
    static void textModelLeave(void *data, struct text_model *text_model);
    static void textModelInputPanelState(void *data, struct text_model *text_model, uint32_t state);
    static void textModelInputPanelRect(void *data, struct text_model *text_model, int32_t x, int32_t y, uint32_t width, uint32_t height);

    void updateModifiersMap(struct wl_array *map);
    Qt::KeyboardModifiers convertNativeModifiersToQt(uint32_t modifiers);

public Q_SLOTS:
    void updateInputPanelRect(const QObject* targetObj, const QRect& rect);

private Q_SLOTS:
    void focusObjectDestroyed(QObject *);

private:
    void cleanup();
    static const struct wl_registry_listener registryListener;
    static const struct text_model_listener textModelListener;

    void ensureWaylandConnection();
    void updateSurroundingText(const QVariant& text, const QVariant& cursor, const QVariant& anchor);
    void resetPreEditData();
    void commitAndReset(bool keepCursorPosition = false);

    void resetInputPanelRect(const QObject* targetObj);

    QObject* m_focusObject;
    wl_display* m_display;
    wl_registry* m_registry;
    struct wl_seat* m_seat;
    text_model_factory* m_textModelFactory;
    struct text_model* m_currentTextModel;

    Qt::InputMethodQueries m_pendingQueries;
    bool m_isQueryPending;
    bool m_isCleanupPending;
    bool m_isActivationPending;
    bool m_modelActivated;
    PreEditData m_preEditData;
    XkbQtModifiersMap m_modifiersMap;
    QRectF m_keyboardRect;
    QRect m_inputPanelRectRequested;

    InputPanelState m_inputPanelState;
};

#endif // WAYLANDINPUTCONTEXT_H
