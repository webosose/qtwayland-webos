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

#ifndef WEBOSSHELLSURFACE_P_H
#define WEBOSSHELLSURFACE_P_H

#include <private/qobject_p.h>
#include <QPointF>

#include <wayland-client.h>
#include <wayland-webos-shell-client-protocol.h>
#include <QtWaylandClient/private/qwaylandwlshellsurface_p.h>

class QPlatformWindow;
class QWaylandWindow;
class WebOSShellSurface;

class WebOSShellSurfacePrivate : public QWaylandWlShellSurface {

    Q_DECLARE_PUBLIC(WebOSShellSurface)

public:
    WebOSShellSurfacePrivate(wl_webos_shell_surface* s, struct ::wl_shell_surface *ws, QPlatformWindow* parent);

    virtual ~WebOSShellSurfacePrivate();


    static inline WebOSShellSurfacePrivate* get(WebOSShellSurface* ss)  {
        return ss->d_func();
    }
    WebOSShellSurface *shellSurface() { return q_ptr; }

    WebOSShellSurface::LocationHints locationHint();
    void setLocationHint(WebOSShellSurface::LocationHints hint);

    WebOSShellSurface::KeyMasks keyMask();
    void setKeyMask(WebOSShellSurface::KeyMasks keyMask);

    void setState(Qt::WindowState state);
    QPointF position() { return m_position; }
    void positionChanged();
    void stateAboutToChange(Qt::WindowState state);

    QVariantMap properties() const;
    QVariant property(const QString &name) const;
    QVariant property(const QString &name, const QVariant &defaultValue) const;
    void setProperty(const QString &name, const QVariant &value);

    void sendProperty(const QString &name, const QVariant &value) Q_DECL_OVERRIDE;

    void setInputRegion(const QRegion& region);

    static const struct wl_webos_shell_surface_listener listener;

    static void state_changed(void *data, struct wl_webos_shell_surface *wl_webos_shell_surface, uint32_t state);
    static void close(void *data, struct wl_webos_shell_surface *wl_webos_shell_surface);
    static void position_changed(void *data, struct wl_webos_shell_surface *wl_webos_shell_surface, int32_t x, int32_t y);
    static void exposed(void *data, struct wl_webos_shell_surface *wl_webos_shell_surface, struct wl_array *rectangles);
    static void state_about_to_change(void *data, struct wl_webos_shell_surface *wl_webos_shell_surface, uint32_t state);

    wl_webos_shell_surface* m_shellSurface;
    QWaylandWindow* m_parent;
    QPointF m_position;
    QVariantMap m_properties;
    WebOSShellSurface::LocationHints m_locationHint;
    WebOSShellSurface::KeyMasks m_keyMask;

    WebOSShellSurface *q_ptr;
    Qt::WindowState m_state;
};

#endif
