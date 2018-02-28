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

#include <QtWaylandClient/private/qwaylanddisplay_p.h>
#include <QtWaylandClient/private/qwaylandwindow_p.h>
#include <QtWaylandClient/private/qwaylandclipboard_p.h>

#include <qpa/qplatforminputcontextfactory_p.h>
#include <qpa/qplatforminputcontext.h>
#ifndef QT_NO_ACCESSIBILITY
#include <qpa/qplatformaccessibility.h>
#endif

#include "webosintegration_p.h"
#include "webosplatformwindow_p.h"
#include "webosnativeinterface_p.h"
#include "weboscursor_p.h"
#include "webosinputdevice_p.h"
#include "webosscreen_p.h"

// configurable with WEBOS_DRAG_DISTANCE
static int s_dragDistance = 10;

WebOSIntegration::WebOSIntegration()
    : QWaylandIntegration(true)
{
    if (qEnvironmentVariableIsSet("WEBOS_DRAG_DISTANCE"))
        s_dragDistance = qgetenv("WEBOS_DRAG_DISTANCE").toInt();

    mNativeInterface = new WebOSNativeInterface(this);
    mDisplay = new QWaylandDisplay(this);
    mClipboard = new QWaylandClipboard(mDisplay);
#ifndef QT_NO_ACCESSIBILITY
    mAccessibility = createPlatformAccessibility();
#endif
}

WebOSIntegration::~WebOSIntegration()
{
}

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

    return new WebOSPlatformWindow(window);
}

QWaylandCursor *WebOSIntegration::createPlatformCursor(QWaylandScreen *screen) const
{
    return new WebOSCursor(screen);
}

QWaylandInputDevice *WebOSIntegration::createInputDevice(QWaylandDisplay *display, uint32_t version, uint32_t id)
{
    return new WebOSInputDevice(display, version, id);
}

QWaylandScreen *WebOSIntegration::createPlatformScreen(QWaylandDisplay *display, int version, uint32_t id) const
{
    return new WebOSScreen(display, version, id);
}

#ifndef QT_NO_ACCESSIBILITY
QPlatformAccessibility *WebOSIntegration::createPlatformAccessibility() const
{
    if (qgetenv("WEBOS_LOAD_ACCESSIBILITY_PLUGIN").toInt() == 1)
        return new QPlatformAccessibility();

    return 0;
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
