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

#include <QScreen>
#include <QtGui/QOpenGLContext>
#include <QtGui/private/qguiapplication_p.h>
#include <QtWaylandClient/private/qwaylandscreen_p.h>
#include <QtWaylandClient/private/qwaylandintegration_p.h>

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QtWaylandEglClientHwIntegration/private/qwaylandglcontext_p.h>
#include <QtWaylandEglClientHwIntegration/private/qwaylandeglclientbufferintegration_p.h>
#else
#include "qwaylandglcontext.h"
#include "qwaylandeglclientbufferintegration.h"
#endif
#include "webosnativeinterface_p.h"
#ifdef HAS_CRIU
#include "webosappsnapshotmanager.h"
#endif

#ifdef HAS_CRIU
static WebOSAppSnapshotManager *s_appSnapshotManager = NULL;
#endif

using QtWaylandClient::QWaylandScreen;

WebOSNativeInterface::WebOSNativeInterface(QWaylandIntegration *integration)
    : QWaylandNativeInterface(integration)
{

}

void *
WebOSNativeInterface::nativeResourceForScreen(const QByteArray &resourceString, QScreen *screen)
{
    QByteArray lowerCaseResource = resourceString.toLower();

    if (lowerCaseResource == "display")
        return static_cast<QWaylandScreen *>(screen->handle())->display()->wl_display();

    return QWaylandNativeInterface::nativeResourceForScreen(resourceString, screen);
}

#ifdef HAS_CRIU
void *
WebOSNativeInterface::nativeResourceForIntegration(const QByteArray &resourceString)
{
    QByteArray lowerCaseResource = resourceString.toLower();

    if (lowerCaseResource == "appsnapshotmanager") {
        if (!s_appSnapshotManager) {
            QWaylandIntegration* wi = static_cast<QWaylandIntegration*>(QGuiApplicationPrivate::platformIntegration());
            s_appSnapshotManager = new WebOSAppSnapshotManager(wi->display(),
                                                               static_cast<QWaylandEglClientBufferIntegration*>(wi->clientBufferIntegration()));
        }
        return s_appSnapshotManager;
    }

    return QWaylandNativeInterface::nativeResourceForIntegration(resourceString);
}
#endif
