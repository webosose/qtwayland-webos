// Copyright (c) 2018-2021 LG Electronics, Inc.
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

#include "webostablet.h"
#include "webostablet_p.h"
#include "securecoding.h"

#include <QByteArray>
#include <QGuiApplication>

#include <qpa/qwindowsysteminterface.h>

#include <QtWaylandClient/private/qwaylanddisplay_p.h>
#include <QtWaylandClient/private/qwaylandwindow_p.h>

#define WEBOSTABLET_VERSION 1

WebOSTabletPrivate::WebOSTabletPrivate(QWaylandDisplay* display, uint32_t id)
    : QtWayland::wl_webos_tablet(display->wl_registry(), uint2int(id), WEBOSTABLET_VERSION)
    , q_ptr(nullptr)
{
}

void WebOSTabletPrivate::webos_tablet_tablet_event(wl_array *uniqueId, int32_t pointerType, int32_t buttons,
                                               wl_fixed_t globalX, wl_fixed_t globalY,
                                               int32_t xTilt, int32_t yTilt,
                                               wl_fixed_t pressure, wl_fixed_t rotation)
{
    QByteArray uniqueIdArray(static_cast<char *>(uniqueId->data), ulong2int(uniqueId->size));
    bool ok = false;
    qint64 uid = 1;
    uid = uniqueIdArray.toLongLong(&ok, 10);

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    int deviceType = int(QInputDevice::DeviceType::Stylus);
#else
    int deviceType = QTabletEvent::Stylus;
#endif

    if (!state.lastReportTool && pointerType)
        QWindowSystemInterface::handleTabletEnterProximityEvent(deviceType, pointerType, uid);
    bool down = buttons & Qt::LeftButton;
    QPointF globalPos(wl_fixed_to_double(globalX), wl_fixed_to_double(globalY));
    QWindowSystemInterface::handleTabletEvent(QGuiApplication::focusWindow(), globalPos, globalPos,
                                              deviceType, pointerType,
                                              (Qt::MouseButton)buttons, wl_fixed_to_double(pressure),
                                              xTilt, yTilt, 0, 0, 0, uid,
                                              qGuiApp->keyboardModifiers());

    if (state.lastReportTool && !pointerType)
        QWindowSystemInterface::handleTabletLeaveProximityEvent(deviceType, pointerType, uid);

    state.lastReportDown = down;
    state.lastReportTool = pointerType;
}

WebOSTabletPrivate::~WebOSTabletPrivate()
{
    q_ptr = nullptr;
}

WebOSTablet::WebOSTablet(QWaylandDisplay* display, uint32_t id)
    : d_ptr(new WebOSTabletPrivate(display, id))
{
    Q_D(WebOSTablet);
    d->q_ptr = this;
}

WebOSTablet::~WebOSTablet()
{
}
