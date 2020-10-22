// Copyright (c) 2015-2020 LG Electronics, Inc.
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

#include "webosinputdevice_p.h"
#include "webosplatformwindow_p.h"
#include <QtWaylandClient/private/qwaylanddisplay_p.h>
#include <QtWaylandClient/private/qwaylandtouch_p.h>
#include <QtGlobal>
#include <QDebug>

#include <QtWaylandClient/private/qwaylanddisplay_p.h>

#include "qtwaylandwebostracer.h"

#if QT_CONFIG(xkbcommon)
#include <xkbcommon/xkbcommon.h>
#endif

WebOSInputDevice::WebOSInputDevice(QWaylandDisplay *display, int version, uint32_t id)
    : QWaylandInputDevice(display, version, id)
    , mTouchRegistered(false)
{

}

QWaylandInputDevice::Keyboard *WebOSInputDevice::createKeyboard(QWaylandInputDevice *device)
{
    PMTRACE_FUNCTION;
    return new WebOSKeyboard(device);
}

QWaylandInputDevice::Pointer *WebOSInputDevice::createPointer(QWaylandInputDevice *device)
{
    PMTRACE_FUNCTION;
    return new WebOSPointer(device);
}

QWaylandInputDevice::Touch *WebOSInputDevice::createTouch(QWaylandInputDevice *device)
{
    PMTRACE_FUNCTION;
    return new WebOSTouch(device);
}

void WebOSInputDevice::seat_capabilities(uint32_t caps)
{
    PMTRACE_FUNCTION;
    if (caps & WL_SEAT_CAPABILITY_TOUCH && !mTouch) {
        // This substitues creation of QTouchDevice in QtWayland
        // Then, we will register it when real event comes up
        mTouchDevice = new QTouchDevice;
        mTouchDevice->setType(QTouchDevice::TouchScreen);
        mTouchDevice->setCapabilities(QTouchDevice::Position);
    }

    QWaylandInputDevice::seat_capabilities(caps);
}

void WebOSInputDevice::registerTouchDevice()
{
    PMTRACE_FUNCTION;
    if (Q_UNLIKELY(!mTouchRegistered) && Q_LIKELY(mTouchDevice)) {
        mTouchRegistered = true;
        QWindowSystemInterface::registerTouchDevice(mTouchDevice);
    }
}

void WebOSInputDevice::setSerial(uint32_t serial)
{
    PMTRACE_FUNCTION;
    mSerial = serial;
}

void WebOSInputDevice::setTime(uint32_t time)
{
    PMTRACE_FUNCTION;
    mTime = time;
}

WebOSInputDevice::WebOSKeyboard::WebOSKeyboard(QWaylandInputDevice *device)
    : Keyboard(device)
{

}

#if QT_CONFIG(xkbcommon)
std::pair<int, QString> WebOSInputDevice::WebOSKeyboard::keysymToQtKey(xkb_keysym_t keysym, Qt::KeyboardModifiers &modifiers)
{
    int code = 0;
    PMTRACE_FUNCTION;
    QString text;
    uint utf32 = xkb_keysym_to_utf32(keysym);
    if (utf32)
        text = QString::fromUcs4(&utf32, 1);

    if (keysym >= XKB_KEY_F1 && keysym <= XKB_KEY_F35) {
        code =  Qt::Key_F1 + (int(keysym) - XKB_KEY_F1);
    } else if (keysym >= XKB_KEY_KP_Space && keysym <= XKB_KEY_KP_9) {
        if (keysym >= XKB_KEY_KP_0) {
            // numeric keypad keys
            code = Qt::Key_0 + ((int)keysym - XKB_KEY_KP_0);
        } else {
            code = Keyboard::keysymToQtKey(keysym);
        }
        modifiers |= Qt::KeypadModifier;
    } else if (text.length() == 1 && text.unicode()->unicode() > 0x1f
            && text.unicode()->unicode() != 0x7f
            && !(keysym >= XKB_KEY_dead_grave && keysym <= XKB_KEY_dead_currency)
            && (int)(keysym) < ((int)Qt::Key_Escape)) {
        code = text.unicode()->toUpper().unicode();
    } else {
        // any other keys
        code = Keyboard::keysymToQtKey(keysym);
    }
    return { (code ? code : keysym), text };
}
#endif

void WebOSInputDevice::WebOSKeyboard::keyboard_key(uint32_t serial, uint32_t time, uint32_t key, uint32_t state)
{
    PMTRACE_FUNCTION;
    Keyboard::keyboard_key(serial, time, key, state);
    //In WebOS, we don't support repeat key by qtwayland
    stopRepeat();
}

WebOSInputDevice::WebOSPointer::WebOSPointer(QWaylandInputDevice *device)
    : Pointer(device)
    , m_origin(QPointF(0, 0))
{
}

void WebOSInputDevice::WebOSPointer::pointer_enter(uint32_t serial, struct wl_surface *surface, wl_fixed_t sx, wl_fixed_t sy)
{
    Q_UNUSED(sx);
    Q_UNUSED(sy);
    PMTRACE_FUNCTION;

    if (!surface)
        return;

    WebOSInputDevice *parent = static_cast<WebOSInputDevice*>(mParent);
    parent->setTime(QWaylandDisplay::currentTimeMillisec());
    parent->setSerial(serial);
    mEnterSerial = serial;

    QWaylandWindow *window = QWaylandWindow::fromWlSurface(surface);

    if (mFocus != window) {
        mFocus = window;
        WebOSPlatformWindow *ww = static_cast<WebOSPlatformWindow *>(mFocus.data());
        m_origin = ww->position();
        connect(ww, &WebOSPlatformWindow::resizeRequested, parent, [this] {
            this->pauseEvents();
        });
        connect(ww, &WebOSPlatformWindow::positionChanged, parent, [this](const QPointF &position) {
            this->flushPausedEvents(position);
        });
    }

    QWaylandWindow *grab = QWaylandWindow::mouseGrab();
    if (!grab) {
        QtWaylandClient::QWaylandPointerEvent enter(QtWaylandClient::QWaylandPointerEvent::Enter, parent->getTime(),
                mSurfacePos, mGlobalPos, mButtons, parent->modifiers());
        window->handleMouse(parent, enter);
    }
}

void WebOSInputDevice::WebOSPointer::pointer_leave(uint32_t time, struct wl_surface *surface)
{
    PMTRACE_FUNCTION;

    WebOSInputDevice *parent = static_cast<WebOSInputDevice*>(mParent);
    WebOSPlatformWindow *ww = static_cast<WebOSPlatformWindow *>(QWaylandWindow::fromWlSurface(surface));
    if (ww)
        ww->disconnect(parent);

    if (Q_UNLIKELY(m_paused))
        flushPausedEvents();

    Pointer::pointer_leave(time, surface);
}

void WebOSInputDevice::WebOSPointer::pointer_motion(uint32_t time, wl_fixed_t surface_x, wl_fixed_t surface_y)
{
    PMTRACE_FUNCTION;

    if (!mFocus)
        return;

    if (Q_UNLIKELY(m_paused)) {
        qDebug() << "Delayed pointer_motion:" << time << wl_fixed_to_double(surface_x) << wl_fixed_to_double(surface_y);
        Event e;
        e.type = Type::motion;
        e.args.motion = {time, surface_x, surface_y};
        m_pendingEvents << e;
        return;
    }

    Pointer::pointer_motion(time, surface_x / mFocus->devicePixelRatio(), surface_y / mFocus->devicePixelRatio());
}

void WebOSInputDevice::WebOSPointer::pointer_button(uint32_t serial, uint32_t time, uint32_t button, uint32_t state)
{
    PMTRACE_FUNCTION;

    if (!mFocus)
        return;

    if (Q_UNLIKELY(m_paused)) {
        qDebug() << "Delayed pointer_button:" << serial << time << button << state;
        Event e;
        e.type = Type::button;
        e.args.button = {serial, time, button, state};
        m_pendingEvents << e;
        return;
    }

    Pointer::pointer_button(serial, time, button, state);
}

void WebOSInputDevice::WebOSPointer::pointer_axis(uint32_t time, uint32_t axis, int32_t value)
{
    PMTRACE_FUNCTION;

    if (!mFocus)
        return;

    if (Q_UNLIKELY(m_paused)) {
        qDebug() << "Delayed pointer_axis:" << time << axis << value;
        Event e;
        e.type = Type::axis;
        e.args.axis = {time, axis, value};
        m_pendingEvents << e;
        return;
    }

    Pointer::pointer_axis(time, axis, value);
}

void WebOSInputDevice::WebOSPointer::pauseEvents()
{
    m_paused = true;
    // Pause events upto the given interval just in case the compositor doesn't respond
    connect(&m_pauseTimer, &QTimer::timeout, static_cast<WebOSInputDevice*>(mParent), [this] {
        this->flushPausedEvents();
    });
    m_pauseTimer.setSingleShot(true);
    m_pauseTimer.setInterval(500);
    m_pauseTimer.start();
    qDebug() << "Start delayed pointer event handling, current origin:" << m_origin;
}

void WebOSInputDevice::WebOSPointer::flushPausedEvents(const QPointF &origin)
{
    QPointF newOrigin = origin;
    if (newOrigin.isNull())
        newOrigin = m_origin;

    if (!m_paused) {
        // Nothing to be done except for updating m_origin
        m_origin = newOrigin;
        return;
    }

    m_paused = false;
    m_pauseTimer.stop();

    qreal dx = m_origin.x() - newOrigin.x();
    qreal dy = m_origin.y() - newOrigin.y();

    // Replay queued events with coordinates translated to newOrigin
    while (!m_pendingEvents.isEmpty()) {
        Event e = m_pendingEvents.takeFirst();
        switch (e.type) {
        case Type::motion:
            qDebug() << "Sending delayed pointer_motion(adjusted):" << e.args.motion.time
                << wl_fixed_to_double(e.args.motion.surface_x) << "+" << dx
                << wl_fixed_to_double(e.args.motion.surface_y) << "+" << dy;
            pointer_motion(e.args.motion.time, e.args.motion.surface_x + wl_fixed_from_double(dx), e.args.motion.surface_y + wl_fixed_from_double(dy));
            break;
        case Type::button:
            qDebug() << "Sending delayed pointer_button:" << e.args.button.serial << e.args.button.time
                << e.args.button.button << e.args.button.state;
            pointer_button(e.args.button.serial, e.args.button.time, e.args.button.button, e.args.button.state);
            break;
        case Type::axis:
            qDebug() << "Sending delayed pointer_axis:" << e.args.axis.time
                << e.args.axis.axis << e.args.axis.value;
            pointer_axis(e.args.axis.time, e.args.axis.axis, e.args.axis.value);
            break;
        }
    }

    qDebug() << "End delayed pointer event handling with origin:" << m_origin << "=>" << newOrigin;

    m_origin = newOrigin;
}

WebOSInputDevice::WebOSTouch::WebOSTouch(QWaylandInputDevice *device)
    : Touch(device)
{
}

void WebOSInputDevice::WebOSTouch::registerTouchDevice()
{
    PMTRACE_FUNCTION;
    WebOSInputDevice *device = static_cast<WebOSInputDevice *>(mParent);
    device->registerTouchDevice();
}

void WebOSInputDevice::WebOSTouch::touch_down(uint32_t serial, uint32_t time, struct wl_surface *surface, int32_t id, wl_fixed_t x, wl_fixed_t y)
{
    PMTRACE_FUNCTION;
    if (!surface)
        return;
    QWaylandWindow *window = QWaylandWindow::fromWlSurface(surface);
    if (!window)
        return;
    Touch::touch_down(serial, time, surface, id, x / window->devicePixelRatio(), y / window->devicePixelRatio());
}

void WebOSInputDevice::WebOSTouch::touch_motion(uint32_t time, int32_t id, wl_fixed_t x, wl_fixed_t y)
{
    PMTRACE_FUNCTION;
    if (!mFocus)
        return;
    Touch::touch_motion(time, id, x / mFocus->devicePixelRatio(), y / mFocus->devicePixelRatio());
}

void WebOSInputDevice::WebOSTouch::touch_cancel()
{
    PMTRACE_FUNCTION;
    registerTouchDevice();
    Touch::touch_cancel();
}

void WebOSInputDevice::WebOSTouch::touch_frame()
{
    PMTRACE_FUNCTION;
    registerTouchDevice();
    Touch::touch_frame();
}
