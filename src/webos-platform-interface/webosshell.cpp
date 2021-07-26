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

#include "webosshell.h"
#include "webosshell_p.h"
#include "webosshellsurface.h"
#include "webosshellsurface_p.h"

#include <QtWaylandClient/private/qwaylanddisplay_p.h>
#include <QtWaylandClient/private/qwaylandwindow_p.h>
#include <QWindow>
#include <QDebug>

using QtWaylandClient::QWaylandWindow;
using QtWaylandClient::QWaylandShellSurface;

static bool platformWindowCreated(QWindow* w)
{
    Q_ASSERT(w);
    bool created = w->handle();
    if (!created) {
        qWarning() << "QPlatformWindow not available for window" << w;
        qWarning() << "Please make the window visible atleast once before aquiring";
        qWarning() << "webOS platform resources";
    }
    return created;
}

WebOSShellPrivate::WebOSShellPrivate(QWaylandDisplay* display, uint32_t id)
    : m_display(display)
{
    m_shell = static_cast<wl_webos_shell*>(wl_registry_bind(display->wl_registry(), id, &wl_webos_shell_interface, 2));
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    display->addRegistryListener(WebOSShellPrivate::registry_global, this);
#endif
}

WebOSShellPrivate::~WebOSShellPrivate()
{
    if (m_wlShell) {
        delete m_wlShell;
        m_wlShell = nullptr;
    }
}

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
void WebOSShellPrivate::registry_global(void *data, struct wl_registry *registry, uint32_t id, const QString &interface, uint32_t version)
{
    WebOSShellPrivate *ssp = static_cast<WebOSShellPrivate *>(data);

    if (interface == "wl_shell")
        ssp->setWlShell(new QtWayland::wl_shell(registry, id, version));
}

QWaylandShellSurface *WebOSShellPrivate::preCreateShellSurface(QWaylandWindow* waylandWindow)
{
    if (m_preCreatedShellSurfaces.contains(waylandWindow)) {
        qInfo() << m_preCreatedShellSurfaces[waylandWindow] << "is already pre-created for" << static_cast<QPlatformWindow *>(waylandWindow);
    } else {
        m_preCreatedShellSurfaces[waylandWindow] = createShellSurface(waylandWindow);
        connect(waylandWindow, &QObject::destroyed, this, [this, waylandWindow] {
            QWaylandShellSurface *ss = m_preCreatedShellSurfaces.take(waylandWindow);
            qInfo() << "Clean up pre-created shell surface" << ss << "for window about to be destroyed" << static_cast<QPlatformWindow *>(waylandWindow);
            delete ss;
        });
    }

    return m_preCreatedShellSurfaces[waylandWindow];
}
#endif

QWaylandShellSurface* WebOSShellPrivate::createShellSurface(QWaylandWindow* waylandWindow)
{
    Q_Q(WebOSShell);

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    if (m_preCreatedShellSurfaces.contains(waylandWindow))
        return m_preCreatedShellSurfaces.take(waylandWindow);
#else
    // Moved from qwaylandwlshellintegration.cpp
    if (m_wlShell) {
        delete m_wlShell;
        m_wlShell = nullptr;
    }

    Q_FOREACH (QWaylandDisplay::RegistryGlobal global, m_display->globals()) {
        if (global.interface == QLatin1String("wl_shell")) {
            m_wlShell = new QtWayland::wl_shell(m_display->wl_registry(), global.id, 1);
            break;
        }
    }
#endif

    if (m_wlShell) {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        struct wl_webos_shell_surface* webos_shell_surface = wl_webos_shell_get_shell_surface(m_shell, waylandWindow->wlSurface());
        struct wl_shell_surface *shell_surface = m_wlShell->get_shell_surface(waylandWindow->wlSurface());
#else
        struct wl_webos_shell_surface* webos_shell_surface = wl_webos_shell_get_shell_surface(m_shell, waylandWindow->object());
        struct wl_shell_surface *shell_surface = m_wlShell->get_shell_surface(waylandWindow->object());
#endif
        if (webos_shell_surface && shell_surface) {
            WebOSShellSurface* wss = new WebOSShellSurface(webos_shell_surface, shell_surface, waylandWindow);
            emit q->shellSurfaceCreated(wss, waylandWindow);
            return WebOSShellSurfacePrivate::get(wss);
        }

        qCritical() << "Failed to create webos_shell_surface shell_surfaces: " <<  webos_shell_surface << shell_surface;
    }

    qCritical() << "Failed to create webos_shell_surface m_wlShell: " << m_wlShell;
    return 0;
}

WebOSShell::WebOSShell(QWaylandDisplay* display, uint32_t id)
    : d_ptr(new WebOSShellPrivate(display, id))
{
    Q_D(WebOSShell);
    d->q_ptr = this;
}

WebOSShell::~WebOSShell()
{
}

WebOSShellSurface* WebOSShell::shellSurfaceFor(QWindow* window)
{
    Q_D(WebOSShell);

    WebOSShellSurfacePrivate* ssp = NULL;
    if (!platformWindowCreated(window))
        return nullptr;

    QWaylandWindow* qww = static_cast<QWaylandWindow*>(window->handle());
    ssp = static_cast<WebOSShellSurfacePrivate *>(qww->shellSurface());

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    if (!ssp)
       ssp =  static_cast<WebOSShellSurfacePrivate *>(d->preCreateShellSurface(qww));
#endif

    return ssp ? ssp->shellSurface() : NULL;
}
