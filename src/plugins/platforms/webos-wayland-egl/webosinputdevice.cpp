// Copyright (c) 2015-2018 LG Electronics, Inc.
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
#include <QtWaylandClient/private/qwaylanddisplay_p.h>
#include <QtWaylandClient/private/qwaylandtouch_p.h>
#include <QtGlobal>
#include <QDebug>

#include <QtWaylandClient/private/qwaylanddisplay_p.h>

#include "qtwaylandwebostracer.h"

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

int WebOSInputDevice::WebOSKeyboard::keysymToQtKey(xkb_keysym_t keysym, Qt::KeyboardModifiers &modifiers, const QString &text)
{
    int code = 0;
    PMTRACE_FUNCTION;

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

    return code ? code : keysym;
}

void WebOSInputDevice::WebOSKeyboard::keyboard_key(uint32_t serial, uint32_t time, uint32_t key, uint32_t state)
{
    PMTRACE_FUNCTION;
    Keyboard::keyboard_key(serial, time, key, state);
    //In WebOS, we don't support repeat key by qtwayland
    stopRepeat();
}

WebOSInputDevice::WebOSPointer::WebOSPointer(QWaylandInputDevice *device)
    : Pointer(device)
{

}

void WebOSInputDevice::WebOSPointer::pointer_enter(uint32_t serial, struct wl_surface *surface, wl_fixed_t sx, wl_fixed_t sy)
{
    Q_UNUSED(sx);
    Q_UNUSED(sy);
    PMTRACE_FUNCTION;

    if (!surface)
        return;

    QWaylandWindow *window = QWaylandWindow::fromWlSurface(surface);

    mFocus = window;

    WebOSInputDevice *parent = static_cast<WebOSInputDevice*>(mParent);
    parent->setTime(QWaylandDisplay::currentTimeMillisec());
    parent->setSerial(serial);
    mEnterSerial = serial;

    QWaylandWindow *grab = QWaylandWindow::mouseGrab();
    if (!grab) {
        window->handleMouseEnter(parent);
        window->handleMouse(parent, parent->getTime(), mSurfacePos, mGlobalPos, mButtons, parent->modifiers());
    }
}

void WebOSInputDevice::WebOSPointer::pointer_motion(uint32_t time, wl_fixed_t surface_x, wl_fixed_t surface_y)
{
    PMTRACE_FUNCTION;
    if (!mFocus)
        return;
    Pointer::pointer_motion(time, surface_x / mFocus->devicePixelRatio(), surface_y / mFocus->devicePixelRatio());
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
