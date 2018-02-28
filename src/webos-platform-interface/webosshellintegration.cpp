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

#include "webosshellintegration.h"
#include "webosshell.h"
#include "webosshell_p.h"
#include "webosplatform.h"
#include "webosplatform_p.h"
#include "webosshellsurface.h"

WebOSShellIntegration::WebOSShellIntegration()
    : m_display(0)
{
}

WebOSShellIntegration::~WebOSShellIntegration()
{
}

bool WebOSShellIntegration::initialize(QWaylandDisplay *display)
{
    m_display = display;

    if (m_display)
        m_display->addRegistryListener(WebOSShellIntegration::registry_global, this);

    return true;
}

QWaylandShellSurface *WebOSShellIntegration::createShellSurface(QWaylandWindow *window)
{
    WebOSShell *webOSShell = WebOSPlatform::instance()->shell();
    if (webOSShell) {
        WebOSShellPrivate* p = WebOSShellPrivate::get(webOSShell);
        return p->createShellSurface(window);
    }

    return 0;
}

void WebOSShellIntegration::registry_global(void *data, struct wl_registry *registry, uint32_t id, const QString &interface, uint32_t version)
{
    WebOSShellIntegration *integration = static_cast<WebOSShellIntegration *>(data);

    if (interface == "wl_webos_shell") {
        WebOSShell *webOSShell = new WebOSShell(integration->display(), id);
        WebOSPlatformPrivate* p = WebOSPlatformPrivate::get(WebOSPlatform::instance());
        p->setShell(webOSShell);
    }
}
