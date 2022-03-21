// Copyright (c) 2020-2022 LG Electronics, Inc.
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

#include <QtCore/qglobal.h>

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QtWaylandEglCompositorHwIntegration/private/waylandeglclientbufferintegration_p.h>
#else
#include <QtWaylandCompositor/private/waylandeglclientbufferintegration.h>
#endif

#if !defined(WEBOS_WAYLANDCOMPOSITOR_EXPORT)
#  if defined(QT_SHARED)
#    define WEBOS_WAYLANDCOMPOSITOR_EXPORT Q_DECL_EXPORT
#  else
#    define WEBOS_WAYLANDCOMPOSITOR_EXPORT
#  endif
#endif

class WebOSEglClientBufferIntegration;

class WEBOS_WAYLANDCOMPOSITOR_EXPORT WebOSEglClientBuffer : public WaylandEglClientBuffer
{
public:
    WebOSEglClientBuffer(WebOSEglClientBufferIntegration* integration, wl_resource *bufferResource);

private:
    WebOSEglClientBufferIntegration *m_webosIntegration = nullptr;
};

class WEBOS_WAYLANDCOMPOSITOR_EXPORT WebOSEglClientBufferIntegration : public WaylandEglClientBufferIntegration
{
public:
    WebOSEglClientBufferIntegration();
};
