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

#ifndef WEBOSEGLPLATFORMINTEGRATION_H
#define WEBOSEGLPLATFORMINTEGRATION_H

#include "webosintegration_p.h"

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QtWaylandEglClientHwIntegration/private/qwaylandeglclientbufferintegration_p.h>
#else
#include "qwaylandeglclientbufferintegration.h"
#endif

QT_BEGIN_NAMESPACE

using QtWaylandClient::QWaylandEglClientBufferIntegration;

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
class WebOSEglClientBufferIntegration : public QWaylandEglClientBufferIntegration
{
public:
    bool supportsThreadedOpenGL() const override
    {
        static QByteArray threaded = qgetenv("WEBOS_GL_DISABLE_THREADED_RENDERING");
        if (threaded.isEmpty())
            return true;
        else
            return false;
    }
};
#endif

class WebOSEglPlatformIntegration : public WebOSIntegration
{
public:
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    WebOSEglPlatformIntegration()
        : WebOSIntegration()
        , m_client_buffer_integration(new WebOSEglClientBufferIntegration())
    {
    }

    void initialize() override
    {
        WebOSIntegration::initialize();
        //Do this after QWaylandIntegration::initialize to get proper display()
        m_client_buffer_integration->initialize(display());
    }
#else
    WebOSEglPlatformIntegration()
        : WebOSIntegration()
        , m_client_buffer_integration(new QWaylandEglClientBufferIntegration())
    {
        m_client_buffer_integration->initialize(display());
    }
#endif

    virtual ~WebOSEglPlatformIntegration()
    {
        delete m_client_buffer_integration;
    }

    QWaylandEglClientBufferIntegration *clientBufferIntegration() const
    { return m_client_buffer_integration; }

private:
    QWaylandEglClientBufferIntegration *m_client_buffer_integration;
};

QT_END_NAMESPACE

#endif
