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

#include "webosplatform.h"
#include "webosplatform_p.h"
#include "webossurfacegroupcompositor.h"
#include "webosshell.h"
#include "webosinputmanager.h"
#include "webosinputpanellocator.h"
#include "webosforeign.h"
#include "webostablet.h"

#include <QDebug>
#include <QtGui/private/qguiapplication_p.h>
#include <QtWaylandClient/private/qwaylandintegration_p.h>
#ifdef HAS_CRIU
#include <qpa/qplatformnativeinterface.h>
#endif

WebOSPlatformPrivate::WebOSPlatformPrivate()
    : m_shell(0)
    , m_groupCompositor(0)
    , m_inputManager(0)
    , m_display(0)
    , m_foreign(0)
#ifdef HAS_CRIU
    , m_appSnapshotManager(0)
#endif
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
    Q_UNUSED(registry);
    Q_UNUSED(version);

    WebOSPlatformPrivate *p = static_cast<WebOSPlatformPrivate *>(data);
    if (interface == "wl_webos_shell") {
        p->setShell(new WebOSShell(p->display(), id));
    } else if (interface == "wl_webos_surface_group_compositor") {
        p->m_groupCompositor = new WebOSSurfaceGroupCompositor(p->display(), id);
    } else if (interface == "wl_webos_input_manager") {
        p->m_inputManager = new WebOSInputManager(p->display(), id);
    } else if (interface == "wl_webos_foreign") {
        p->m_foreign = new WebOSForeign(p->display(), id);
    } else if (interface == "wl_webos_tablet") {
        bool ok = false;
        int webos_tablet = qEnvironmentVariableIntValue("WEBOS_TABLET", &ok);
        if (ok && webos_tablet) {
            qInfo() << "Binding wl_webos_tablet as WEBOS_TABLET is set to" << webos_tablet;
            p->m_webosTablet = new WebOSTablet(p->display(), id);
        }
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

WebOSForeign* WebOSPlatform::webOSForeign()
{
    Q_D(WebOSPlatform);
    if (d->m_foreign)
        return d->m_foreign;
    qWarning("No WebOSForeign available");
    return nullptr;
}

WebOSTablet* WebOSPlatform::webOSTablet()
{
    Q_D(WebOSPlatform);
    return d->m_webosTablet;
}

#ifdef HAS_CRIU
AppSnapshotManager* WebOSPlatform::appSnapshotManager()
{
    Q_D(WebOSPlatform);
    if (!d->m_appSnapshotManager) {
        d->m_appSnapshotManager =
            static_cast<AppSnapshotManager*>(QGuiApplication::platformNativeInterface()->nativeResourceForIntegration(QByteArrayLiteral("appsnapshotmanager")));

        QObject::connect(d->m_appSnapshotManager, SIGNAL(stateChanged(const AppSnapshotManager::AppSnapShotState, const QString)),
                         this, SLOT(onStateChanged(const AppSnapshotManager::AppSnapShotState, const QString)));
    }
    return d->m_appSnapshotManager;
}

void WebOSPlatform::onStateChanged(const AppSnapshotManager::AppSnapShotState state, const QString failureReason)
{
    Q_D(WebOSPlatform);

    switch (state) {
        case AppSnapshotManager::Ready:
            delete d->m_groupCompositor;
            delete d->m_inputManager;
            break;
        case AppSnapshotManager::PostDumped:
        case AppSnapshotManager::Restored:
            //RISK : if uderlying QPA is not qtwayland, this will cause problem.
            //Currently no good way is found to validate QPA.
            //QGuiApplication::platformName() was tried, but it returned Null QString.
            if (d->m_display)
                d->m_display->addRegistryListener(WebOSPlatformPrivate::registry_global, d);
            break;
        default:
            break;
    }
}
#endif
