// Copyright (c) 2018-2020 LG Electronics, Inc.
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

#ifndef WEBOSTABLET_P_H
#define WEBOSTABLET_P_H

#include <QObject>

#include "qwayland-webos-tablet.h"

namespace QtWaylandClient {
    class QWaylandDisplay;
}

using QtWaylandClient::QWaylandDisplay;

class WebOSTabletPrivate : public QObject, QtWayland::wl_webos_tablet
{
public:
    WebOSTabletPrivate(QWaylandDisplay* display, uint32_t id);

    WebOSTablet *q_ptr;
    Q_DECLARE_PUBLIC(WebOSTablet)

    virtual ~WebOSTabletPrivate();

    struct {
        bool lastReportDown = false;
        int lastReportTool = 0;
    } state;

protected:
    virtual void webos_tablet_tablet_event(wl_array *uniqueId, int32_t pointerType, int32_t down, wl_fixed_t globalX, wl_fixed_t globalY, int32_t xTilt, int32_t yTilt, wl_fixed_t pressure, wl_fixed_t rotation) override;
};

#endif
