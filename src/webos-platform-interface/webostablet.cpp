/* @@@LICENSE
 *
 *      Copyright (c) 2018-2020 LG Electronics, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * LICENSE@@@ */

#include "webostablet.h"
#include "webostablet_p.h"

#include <QGuiApplication>

#include <qpa/qwindowsysteminterface.h>

#include <QtWaylandClient/private/qwaylanddisplay_p.h>
#include <QtWaylandClient/private/qwaylandwindow_p.h>

#define WEBOSTABLET_VERSION 1

WebOSTabletPrivate::WebOSTabletPrivate(QWaylandDisplay* display, uint32_t id)
    : QtWayland::wl_webos_tablet(display->wl_registry(), id, WEBOSTABLET_VERSION)
    , q_ptr(0)
{
}

void WebOSTabletPrivate::webos_tablet_tablet_event(int32_t uniqueId, int32_t pointerType, int32_t down,
                                               wl_fixed_t globalX, wl_fixed_t globalY,
                                               uint32_t xTilt, uint32_t yTilt,
                                               wl_fixed_t pressure, wl_fixed_t rotation)
{
    if (!state.lastReportTool && pointerType)
        QWindowSystemInterface::handleTabletEnterProximityEvent(QTabletEvent::Stylus, pointerType, uniqueId);

    QPointF globalPos(wl_fixed_to_double(globalX), wl_fixed_to_double(globalY));
    QWindowSystemInterface::handleTabletEvent(0, down, QPointF(), globalPos,
                                              QTabletEvent::Stylus, pointerType,
                                              wl_fixed_to_double(pressure),
                                              0, 0, 0, 0, 0, 1, qGuiApp->keyboardModifiers());

    if (state.lastReportTool && !pointerType)
        QWindowSystemInterface::handleTabletLeaveProximityEvent(QTabletEvent::Stylus, pointerType, uniqueId);

    state.lastReportDown = down;
    state.lastReportTool = pointerType;
}

WebOSTabletPrivate::~WebOSTabletPrivate()
{
    q_ptr = NULL;
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
