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

#include "webosplatform.h"
#include "webosplatform_p.h"
#include "webossurfacegroupcompositor.h"
#include "webosshell.h"
#include "webosinputmanager.h"
#include "webosinputpanellocator.h"

#include <QDebug>
#include <QtGui/private/qguiapplication_p.h>
#include <QtWaylandClient/private/qwaylandintegration_p.h>

WebOSPlatformPrivate::WebOSPlatformPrivate()
    : m_shell(0)
    , m_groupCompositor(0)
    , m_inputManager(0)
    , m_display(0)
{
    //RISK : if uderlying QPA is not qtwayland, this will cause problem.
    //Currently no good way is found to validate QPA.
    //QGuiApplication::platformName() was tried, but it returned Null QString.
    m_display = static_cast<QWaylandIntegration *>(QGuiApplicationPrivate::platformIntegration())->display();
    if (m_display)
        m_display->addRegistryListener(WebOSPlatformPrivate::registry_global, this);
}

WebOSPlatformPrivate::~WebOSPlatformPrivate()
{
}

void WebOSPlatformPrivate::registry_global(void *data, struct wl_registry *registry, uint32_t id, const QString &interface, uint32_t version)
{
    WebOSPlatformPrivate *p = static_cast<WebOSPlatformPrivate *>(data);
    if (interface == "wl_webos_surface_group_compositor") {
        p->m_groupCompositor = new WebOSSurfaceGroupCompositor(p->display(), id);
    } else if (interface == "wl_webos_input_manager") {
        p->m_inputManager = new WebOSInputManager(p->display(), id);
    }
}

void WebOSPlatformPrivate::setShell(WebOSShell* shell)
{
    m_shell = shell;
}

WebOSPlatform::WebOSPlatform()
    : d_ptr(new WebOSPlatformPrivate)
{
}

// WebOSPlatform should be created in instance(), otherwise the instance is created
// eventhogh it is not used at all. ex) Some qml module links webos-platform-interface.
WebOSPlatform* WebOSPlatform::m_instance = 0;

WebOSPlatform::~WebOSPlatform()
{
}

WebOSPlatform* WebOSPlatform::instance()
{
    if (Q_UNLIKELY(!m_instance))
        m_instance = new WebOSPlatform;
    return m_instance;
}

WebOSShell* WebOSPlatform::shell()
{
    Q_D(WebOSPlatform);
    return d->m_shell;
}

WebOSSurfaceGroupCompositor* WebOSPlatform::surfaceGroupCompositor()
{
    Q_D(WebOSPlatform);
    if (d->m_groupCompositor) {
        return d->m_groupCompositor;
    }
    qWarning("No surface group compositor available");
    return NULL;
}

WebOSInputManager* WebOSPlatform::inputManager()
{
    Q_D(WebOSPlatform);
    return d->m_inputManager;
}

WebOSInputPanelLocator* WebOSPlatform::inputPanelLocator()
{
    return WebOSInputPanelLocator::instance();
}
