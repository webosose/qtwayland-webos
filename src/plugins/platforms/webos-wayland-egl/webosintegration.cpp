// Copyright (c) 2015-2021 LG Electronics, Inc.
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

#include <QtWaylandClient/private/qwaylanddisplay_p.h>
#include <QtWaylandClient/private/qwaylandwindow_p.h>
#include <QtWaylandClient/private/qwaylandclipboard_p.h>

#include <qpa/qplatforminputcontextfactory_p.h>
#include <qpa/qplatforminputcontext.h>

#include "webosintegration_p.h"
#include "webosplatformwindow_p.h"
#include "webosnativeinterface_p.h"
#include "weboscursor_p.h"
#include "webosinputdevice_p.h"
#include "webosscreen_p.h"

// configurable with WEBOS_DRAG_DISTANCE
static int s_dragDistance = 10;

using QtWaylandClient::QWaylandClipboard;

WebOSIntegration::WebOSIntegration()
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    : QWaylandIntegration()
#else
    : QWaylandIntegration(true)
#endif
{
    if (qEnvironmentVariableIsSet("WEBOS_DRAG_DISTANCE"))
        s_dragDistance = qgetenv("WEBOS_DRAG_DISTANCE").toInt();

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    mNativeInterface.reset(new WebOSNativeInterface(this));
    mDisplay.reset(new QWaylandDisplay(this));
    mClipboard.reset(new QWaylandClipboard(mDisplay.data()));
#endif
}

WebOSIntegration::~WebOSIntegration()
{
    mInputContext.reset(nullptr);
}

QPlatformWindow *WebOSIntegration::createPlatformWindow(QWindow *window) const
{
    // If one of conditions below is not satisfied,
    // we should abort further initialization right away
    // rather than trying a fallback which may lead an unexpected result.
    // Note that abort() doesn't work properly as well for some reason,
    // thus we call ::exit() instead to terminate the process gracefully.

    if ((window->surfaceType() != QWindow::OpenGLSurface && window->surfaceType() != QWindow::RasterGLSurface)) {
        qCritical("Unsupported surfaceType(%d), exiting.", window->surfaceType());
        ::exit(1);
    }

    QByteArray integrationName = qgetenv("QT_WAYLAND_SHELL_INTEGRATION");
    if (integrationName.isEmpty()) {
        qWarning("QT_WAYLAND_SHELL_INTEGRATION is not set. Setting as \"webos\".");
        qputenv("QT_WAYLAND_SHELL_INTEGRATION", "webos");
    } else if (qstrcmp(integrationName, "webos")) {
        qCritical("Unsupported shell integration(QT_WAYLAND_SHELL_INTEGRATION=%s), exiting", integrationName.data());
        ::exit(1);
    }

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    // This will initialize shellIntegration if it is not ready yet
    if (!shellIntegration()) {
        qCritical("Shell integration is not ready", integrationName.data());
        ::exit(1);
    }

    return new WebOSPlatformWindow(window, mDisplay.data());
#else
    return new WebOSPlatformWindow(window);
#endif
}

QWaylandScreen *WebOSIntegration::createPlatformScreen(QWaylandDisplay *display, int version, uint32_t id) const
{
    return new WebOSScreen(display, version, id);
}

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
QWaylandCursor *WebOSIntegration::createPlatformCursor(QWaylandDisplay *display) const
{
    return new WebOSCursor(display);
}
#else
QWaylandCursor *WebOSIntegration::createPlatformCursor(QWaylandScreen *screen) const
{
    return new WebOSCursor(screen);
}
#endif

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
QWaylandInputDevice *WebOSIntegration::createInputDevice(QWaylandDisplay *display, int version, uint32_t id) const
#else
QWaylandInputDevice *WebOSIntegration::createInputDevice(QWaylandDisplay *display, int version, uint32_t id)
#endif
{
    return new WebOSInputDevice(display, version, id);
}

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
QWaylandNativeInterface *WebOSIntegration::createPlatformNativeInterface()
{
    return new WebOSNativeInterface(this);
}
#endif

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
void WebOSIntegration::initialize()
{
    // TODO
    // Consider creating WaylandInputContext directly like other platform
    // features rather than creating it via QPlatformInputContextFactory.
    // We can pass QWaylandDisplay as constructor param so that it doesn't
    // have to rely on the nativeInterface to get the wl_display.
    // (See QWaylandInputContext as a reference.)
    mInputContext.reset(QPlatformInputContextFactory::create());

    QWaylandIntegration::initialize();
}
#endif

#ifdef HAS_CRIU
void WebOSIntegration::resetInputContext()
{
    mInputContext.reset();
}
#endif

QVariant WebOSIntegration::styleHint(StyleHint hint) const
{
    switch (hint) {
    case StartDragDistance:
        return s_dragDistance;
    default:
        return QWaylandIntegration::styleHint(hint);
    }
}

bool WebOSIntegration::hasCapability(QPlatformIntegration::Capability cap) const
{
    switch (cap) {
    case MultipleWindows:
    case NonFullScreenWindows:
        return false;
    default: return QWaylandIntegration::hasCapability(cap);
    }
}
