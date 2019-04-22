// Copyright (c) 2015-2020 LG Electronics, Inc.
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

#ifndef WEBOSINTEGRATION_H
#define WEBOSINTEGRATION_H

#include <QtWaylandClient/private/qwaylandintegration_p.h>
#include <QtWaylandClient/private/qwaylandscreen_p.h>

using QtWaylandClient::QWaylandIntegration;
using QtWaylandClient::QWaylandCursor;
using QtWaylandClient::QWaylandScreen;
using QtWaylandClient::QWaylandInputDevice;
using QtWaylandClient::QWaylandDisplay;

class WebOSIntegration : public QWaylandIntegration
{
public:
    WebOSIntegration();
    ~WebOSIntegration();

    void initialize() override;

#ifdef HAS_CRIU
    void resetInputContext();
#endif

    QPlatformWindow *createPlatformWindow(QWindow *window) const Q_DECL_OVERRIDE;
    QWaylandCursor *createPlatformCursor(QWaylandScreen *screen) const Q_DECL_OVERRIDE;
    QWaylandScreen *createPlatformScreen(QWaylandDisplay *display, int version, uint32_t id) const Q_DECL_OVERRIDE;
    QWaylandInputDevice *createInputDevice(QWaylandDisplay *display, int version, uint32_t id) Q_DECL_OVERRIDE;
#ifndef QT_NO_ACCESSIBILITY
    QPlatformAccessibility *createPlatformAccessibility() const Q_DECL_OVERRIDE;
#endif

    QVariant styleHint(StyleHint hint) const Q_DECL_OVERRIDE;
    bool hasCapability(QPlatformIntegration::Capability cap) const Q_DECL_OVERRIDE;
};

#endif
