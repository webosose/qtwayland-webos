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

#ifndef WEBOSINTEGRATION_H
#define WEBOSINTEGRATION_H

#include <QtWaylandClient/private/qwaylandintegration_p.h>
#include <QtWaylandClient/private/qwaylandscreen_p.h>
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QtWaylandClient/private/qwaylandnativeinterface_p.h>
#endif

using QtWaylandClient::QWaylandIntegration;
using QtWaylandClient::QWaylandCursor;
using QtWaylandClient::QWaylandScreen;
using QtWaylandClient::QWaylandInputDevice;
using QtWaylandClient::QWaylandDisplay;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
using QtWaylandClient::QWaylandNativeInterface;
#endif

class WebOSIntegration : public QWaylandIntegration
{
public:
    WebOSIntegration();
    ~WebOSIntegration();

    QPlatformWindow *createPlatformWindow(QWindow *window) const override;
    QWaylandScreen *createPlatformScreen(QWaylandDisplay *display, int version, uint32_t id) const override;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    QWaylandCursor *createPlatformCursor(QWaylandDisplay *display) const override;
    QWaylandInputDevice *createInputDevice(QWaylandDisplay *display, int version, uint32_t id) const override;
    QWaylandNativeInterface *createPlatformNativeInterface() override;
#else
    QWaylandCursor *createPlatformCursor(QWaylandScreen *screen) const override;
    QWaylandInputDevice *createInputDevice(QWaylandDisplay *display, int version, uint32_t id) override;
    void initialize() override;
#endif

#ifdef HAS_CRIU
    void resetInputContext();
#endif

    QVariant styleHint(StyleHint hint) const override;
    bool hasCapability(QPlatformIntegration::Capability cap) const override;
};

#endif
