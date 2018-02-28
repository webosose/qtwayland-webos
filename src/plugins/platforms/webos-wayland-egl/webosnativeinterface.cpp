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

#include <QScreen>
#include <QtGui/QOpenGLContext>
#include <QtWaylandClient/private/qwaylandscreen_p.h>

#include "qwaylandglcontext.h"
#include "webosnativeinterface_p.h"

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
