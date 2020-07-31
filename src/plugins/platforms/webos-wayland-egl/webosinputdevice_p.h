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

#ifndef WEBOSINPUTDEVICE_H
#define WEBOSINPUTDEVICE_H

#include <QtWaylandClient/private/qwaylandinputdevice_p.h>

using QtWaylandClient::QWaylandInputDevice;
using QtWaylandClient::QWaylandDisplay;
using QtWaylandClient::QWaylandWindow;

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

#if QT_CONFIG(xkbcommon)
    std::pair<int, QString> keysymToQtKey(xkb_keysym_t keysym, Qt::KeyboardModifiers &modifiers) override;
#endif

    void keyboard_key(uint32_t serial, uint32_t time, uint32_t key, uint32_t state) Q_DECL_OVERRIDE;
};

class WebOSInputDevice::WebOSPointer : public QWaylandInputDevice::Pointer
{
public:
    WebOSPointer(QWaylandInputDevice *device);

    void pointer_enter(uint32_t serial, struct wl_surface *surface, wl_fixed_t sx, wl_fixed_t sy) override;
    void pointer_leave(uint32_t time, struct wl_surface *surface) override;
    void pointer_motion(uint32_t time, wl_fixed_t surface_x, wl_fixed_t surface_y) override;
    void pointer_button(uint32_t serial, uint32_t time, uint32_t button, uint32_t state) override;
    void pointer_axis(uint32_t time, uint32_t axis, int32_t value) override;

    void pauseEvents();
    void flushPausedEvents(const QPointF &origin = QPointF());

private:
    // Only events to be delayed
    enum Type {
        motion,
        button,
        axis
    };

    struct Event {
        Type type;
        union {
            struct {
                uint32_t time;
                wl_fixed_t surface_x;
                wl_fixed_t surface_y;
            } motion;
            struct {
                uint32_t serial;
                uint32_t time;
                uint32_t button;
                uint32_t state;
            } button;
            struct {
                uint32_t time;
                uint32_t axis;
                int32_t value;
            } axis;
        } args;
    };

    QPointF m_origin;
    bool m_paused = false;
    QVector<Event> m_pendingEvents;
    QTimer m_pauseTimer;
};

class WebOSInputDevice::WebOSTouch : public QWaylandInputDevice::Touch
{
public:
    WebOSTouch(QWaylandInputDevice *device);

    void registerTouchDevice();
    void touch_down(uint32_t serial, uint32_t time, struct wl_surface *surface, int32_t id, wl_fixed_t x, wl_fixed_t y) Q_DECL_OVERRIDE;
    void touch_motion(uint32_t time, int32_t id, wl_fixed_t x, wl_fixed_t y) Q_DECL_OVERRIDE;
    void touch_cancel() Q_DECL_OVERRIDE;
    void touch_frame() Q_DECL_OVERRIDE;
};
#endif
