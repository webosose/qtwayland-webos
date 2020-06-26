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

#include <QtWaylandCompositor/private/waylandeglclientbufferintegration.h>

#include <QQuickItem>

class WebOSEglClientBufferIntegration;

class WebOSEglClientBuffer : public WaylandEglClientBuffer
{
public:
    WebOSEglClientBuffer(WebOSEglClientBufferIntegration* integration, wl_resource *bufferResource);
    bool directUpdate(QQuickItem *item, uint32_t zpos) override;

private:
    WebOSEglClientBufferIntegration *m_webosIntegration = nullptr;
};

class WebOSEglClientBufferIntegration : public WaylandEglClientBufferIntegration
{
public:
    WebOSEglClientBufferIntegration();

    void initializeHardware(struct ::wl_display *display) override;
    QtWayland::ClientBuffer *createBufferFor(wl_resource *buffer) override;
    bool isSecured(struct ::wl_resource *buffer) override;

    bool supportsSetOverlayBufferObject();

    bool directUpdate(QQuickItem *item, uint32_t zpos, QtWayland::ClientBuffer *buffer) override;
private:
    void loadExternalBufferIntegration();

private:
    QScopedPointer<QtWayland::ClientBufferIntegration> m_externalBufferIntegration;
};

