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

#ifndef WEBOSSURFACEGROUP_P_H
#define WEBOSSURFACEGROUP_P_H

#include "qwayland-webos-surface-group.h"
#include "webossurfacegroup.h"

#include <QObject>
#include <QList>
#include <QPointer>

class WebOSSurfaceGroup;
class WebOSSurfaceGroupLayer;

namespace QtWaylandClient {
    class QWaylandWindow;
}

using QtWaylandClient::QWaylandWindow;

class WebOSSurfaceGroupPrivate : public QObject, public QtWayland::wl_webos_surface_group {

    Q_OBJECT
    Q_DECLARE_PUBLIC(WebOSSurfaceGroup)

public:
    WebOSSurfaceGroupPrivate();
    ~WebOSSurfaceGroupPrivate();

    void setAllowAnonymousLayers(bool allow);
    void attachAnonymousSurface(QWaylandWindow* surface, WebOSSurfaceGroup::ZHint hint);
    WebOSSurfaceGroupLayer* createLayer(const QString& name, int z);
    void attachSurface(QWaylandWindow* surface, const QString& layer);
    void detachSurface(QWaylandWindow* surface);

    void focusOwner();
    void focusLayer(const QString& layerName);

    void commitKeyIndex(bool commit);

    void webos_surface_group_owner_destroyed();

    static inline WebOSSurfaceGroupPrivate* get(WebOSSurfaceGroup* group) {
        return group->d_func();
    }

    WebOSSurfaceGroup* q_ptr;

private:
    QList<QPointer<QWaylandWindow> > m_attachedSurfaces;
};

#endif
