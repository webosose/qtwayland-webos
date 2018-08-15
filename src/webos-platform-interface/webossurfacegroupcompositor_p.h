// Copyright (c) 2013-2019 LG Electronics, Inc.
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

#ifndef WEBOSSURFACEGROUPCOMPOSITOR_P_H
#define WEBOSSURFACEGROUPCOMPOSITOR_P_H

#include "qwayland-webos-surface-group.h"

#include <QObject>

class WebOSSurfaceGroup;

namespace QtWaylandClient {
    class QWaylandWindow;
}

using QtWaylandClient::QWaylandWindow;

class WebOSSurfaceGroupCompositorPrivate : public QObject, public QtWayland::wl_webos_surface_group_compositor {

    Q_OBJECT

public:
    WebOSSurfaceGroupCompositorPrivate(struct wl_registry* registry, uint32_t id);

    WebOSSurfaceGroup* createGroup(QWaylandWindow* window, const QString &name);
    WebOSSurfaceGroup* getGroup(const QString &name);
};

#endif
