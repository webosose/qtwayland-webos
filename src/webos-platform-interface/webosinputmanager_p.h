// Copyright (c) 2014-2019 LG Electronics, Inc.
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

#ifndef WEBOSSINPUTMANAGER_P_H
#define WEBOSSINPUTMANAGER_P_H

#include <QObject>
#include <private/qobject_p.h>

#include "qwayland-webos-input-manager.h"

namespace QtWaylandClient {
    class QWaylandDisplay;
}

using QtWaylandClient::QWaylandDisplay;

class WebOSInputManagerPrivate : public QObject, QtWayland::wl_webos_input_manager {

public:
    WebOSInputManagerPrivate(QWaylandDisplay* display, uint32_t id);
    void visibleChanged(bool visible);

    WebOSInputManager *q_ptr;
    Q_DECLARE_PUBLIC(WebOSInputManager)

    virtual ~WebOSInputManagerPrivate();

protected:
    void webos_input_manager_cursor_visibility(uint32_t visibility, struct ::wl_webos_seat *webos_seat);
};
#endif
