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

#ifndef WEBOSSHELL_P_H
#define WEBOSSHELL_P_H

#include <QObject>
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QHash>
#endif
#include <QWindow>
#include <QtWaylandClient/private/qwayland-wayland.h>

#include <wayland-client.h>
#include <wayland-webos-shell-client-protocol.h>

class WebOSShell;
class WebOSShellSurface;
class QWindow;

namespace QtWaylandClient {
    class QWaylandShellSurface;
    class QWaylandDisplay;
}

using QtWaylandClient::QWaylandDisplay;
using QtWaylandClient::QWaylandShellSurface;

class WebOSShellPrivate
{
    Q_DECLARE_PUBLIC(WebOSShell)

public:
    WebOSShellPrivate(QWaylandDisplay* display, uint32_t id);
    virtual ~WebOSShellPrivate();
    WebOSShellPrivate(const WebOSShellPrivate&) = delete;
    WebOSShellPrivate &operator=(const WebOSShellPrivate&) = delete;

    static inline WebOSShellPrivate* get(WebOSShell* shell) {
        return shell->d_func();
    }

    QWaylandShellSurface* createShellSurface(QPlatformWindow* window);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    QWaylandShellSurface* preCreateShellSurface(QPlatformWindow* window);
    void setWlShell(QtWayland::wl_shell *wlShell) { m_wlShell = wlShell; }

    static void registry_global(void *data, struct wl_registry *registry, uint32_t id, const QString &interface, uint32_t version);
#endif

private:
    WebOSShell *q_ptr;
    wl_webos_shell *m_shell = nullptr;
    QtWayland::wl_shell *m_wlShell = nullptr;
    QWaylandDisplay *m_display = nullptr;

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    QHash<QPlatformWindow *, QWaylandShellSurface *> m_preCreatedShellSurfaces;
#endif
};


#endif
