// Copyright (c) 2020 LG Electronics, Inc.
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

#include <QtWaylandCompositor/private/qwlclientbufferintegrationfactory_p.h>

#include "weboseglclientbufferintegration.h"

WebOSEglClientBufferIntegration::WebOSEglClientBufferIntegration()
    : WaylandEglClientBufferIntegration()
{
    loadExternalBufferIntegration();
}

void
WebOSEglClientBufferIntegration::initializeHardware(struct ::wl_display *display)
{
    WaylandEglClientBufferIntegration::initializeHardware(display);

    if (m_externalBufferIntegration)
            m_externalBufferIntegration->initializeHardware(display);
}

QtWayland::ClientBuffer *
WebOSEglClientBufferIntegration::createBufferFor(wl_resource *buffer)
{
    QtWayland::ClientBuffer *cBuffer = nullptr;

    if (m_externalBufferIntegration && (cBuffer = m_externalBufferIntegration->createBufferFor(buffer)))
        return cBuffer;

    return WaylandEglClientBufferIntegration::createBufferFor(buffer);
}

bool WebOSEglClientBufferIntegration::isSecured(struct ::wl_resource *buffer)
{
    if (m_externalBufferIntegration) {
        return m_externalBufferIntegration->isSecured(buffer);
    }

    return false;
}

void WebOSEglClientBufferIntegration::loadExternalBufferIntegration()
{
    QStringList keys = QtWayland::ClientBufferIntegrationFactory::keys();
    QString targetKey;
    QByteArray clientBufferIntegration = qgetenv("WEBOS_EXTERNAL_BUFFER_INTEGRATION");
    if (keys.contains(QString::fromLocal8Bit(clientBufferIntegration.constData()))) {
        targetKey = QString::fromLocal8Bit(clientBufferIntegration.constData());
    }

    if (!targetKey.isEmpty()) {
        m_externalBufferIntegration.reset(QtWayland::ClientBufferIntegrationFactory::create(targetKey, QStringList()));
        if (m_externalBufferIntegration) {
            //m_externalBufferIntegration->setCompositor(q); TODO: Need it?
       }
    }
}

