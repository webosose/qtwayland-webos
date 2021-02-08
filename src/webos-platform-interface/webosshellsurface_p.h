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

#ifndef WEBOSSHELLSURFACE_P_H
#define WEBOSSHELLSURFACE_P_H

#include <private/qobject_p.h>
#include <QPointF>

#include <wayland-client.h>
#include <wayland-webos-shell-client-protocol.h>
#if QT_VERSION >= QT_VERSION_CHECK(6, 2, 0)
#include <QtWlShellIntegration/private/qwaylandwlshellsurface_p.h>
#elif QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QtWlShellSurface/private/qwaylandwlshellsurface_p.h>
#else
#include <QtWaylandClient/private/qwaylandwlshellsurface_p.h>
#endif

class QPlatformWindow;
class WebOSShellSurface;

namespace QtWaylandClient {
    class QWaylandWindow;
}

using QtWaylandClient::QWaylandWindow;
using QtWaylandClient::QWaylandWlShellSurface;

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

    QString addon() const;
    void setAddon(const QString& addon);
    void resetAddon();

#if (QT_VERSION < QT_VERSION_CHECK(5,10,0))
    void setState(Qt::WindowState state);
#else
    void setState(Qt::WindowStates state);
#endif
    QPointF position() { return m_position; }
    void positionChanged();
    void stateAboutToChange(Qt::WindowState state);
    void addonStatusChanged(WebOSShellSurface::AddonStatus status);

    QVariantMap properties() const;
    QVariant property(const QString &name) const;
    QVariant property(const QString &name, const QVariant &defaultValue) const;
    void setProperty(const QString &name, const QVariant &value);

    void sendProperty(const QString &name, const QVariant &value) override;

    void setInputRegion(const QRegion& region);

    static const struct wl_webos_shell_surface_listener listener;

    static void state_changed(void *data, struct wl_webos_shell_surface *wl_webos_shell_surface, uint32_t state);
    static void close(void *data, struct wl_webos_shell_surface *wl_webos_shell_surface);
    static void position_changed(void *data, struct wl_webos_shell_surface *wl_webos_shell_surface, int32_t x, int32_t y);
    static void exposed(void *data, struct wl_webos_shell_surface *wl_webos_shell_surface, struct wl_array *rectangles);
    static void state_about_to_change(void *data, struct wl_webos_shell_surface *wl_webos_shell_surface, uint32_t state);
    static void addon_status_changed(void *data, struct wl_webos_shell_surface *wl_webos_shell_surface, uint32_t addon_status);

    wl_webos_shell_surface* m_shellSurface;
    QWaylandWindow* m_parent;
    QPointF m_position;
    QVariantMap m_properties;
    WebOSShellSurface::LocationHints m_locationHint;
    WebOSShellSurface::KeyMasks m_keyMask;

    WebOSShellSurface *q_ptr;
#if (QT_VERSION < QT_VERSION_CHECK(5,10,0))
    Qt::WindowState m_state;
#else
    Qt::WindowStates m_state;
#endif
    QString m_addon;
};

#endif
