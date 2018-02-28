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

#ifndef WEBOSINPUTDEVICE_H
#define WEBOSINPUTDEVICE_H

#include <QtWaylandClient/private/qwaylandinputdevice_p.h>

class WebOSInputDevice : public QWaylandInputDevice
{
public:
    class WebOSKeyboard;
    class WebOSPointer;
    class WebOSTouch;

    WebOSInputDevice(QWaylandDisplay *display, int version, uint32_t id);

    QWaylandInputDevice::Keyboard *createKeyboard(QWaylandInputDevice *device) Q_DECL_OVERRIDE;
    QWaylandInputDevice::Pointer *createPointer(QWaylandInputDevice *device) Q_DECL_OVERRIDE;
    QWaylandInputDevice::Touch *createTouch(QWaylandInputDevice *device) Q_DECL_OVERRIDE;

    void seat_capabilities(uint32_t caps) Q_DECL_OVERRIDE;
    void registerTouchDevice();

    bool mTouchRegistered;

    friend class WebOSTouch;

    void setSerial(uint32_t serial);
    void setTime(uint32_t time);
    inline uint32_t getTime() const { return mTime; }
};

class WebOSInputDevice::WebOSKeyboard : public QWaylandInputDevice::Keyboard
{
public:
    WebOSKeyboard(QWaylandInputDevice *device);

    int keysymToQtKey(xkb_keysym_t keysym, Qt::KeyboardModifiers &modifiers, const QString &text) Q_DECL_OVERRIDE;

    void keyboard_key(uint32_t serial, uint32_t time, uint32_t key, uint32_t state) Q_DECL_OVERRIDE;
};

class WebOSInputDevice::WebOSPointer : public QWaylandInputDevice::Pointer
{
public:
    WebOSPointer(QWaylandInputDevice *device);

    void pointer_enter(uint32_t serial, struct wl_surface *surface, wl_fixed_t sx, wl_fixed_t sy);
    void pointer_motion(uint32_t time, wl_fixed_t surface_x, wl_fixed_t surface_y);
};

class WebOSInputDevice::WebOSTouch : public QWaylandInputDevice::Touch
{
public:
    WebOSTouch(QWaylandInputDevice *device);

    void registerTouchDevice();
    void touch_cancel() Q_DECL_OVERRIDE;
    void touch_frame() Q_DECL_OVERRIDE;
};
#endif
