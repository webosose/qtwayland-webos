// Copyright (c) 2014-2021 LG Electronics, Inc.
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

#include "webossurfacegroup.h"
#include "webossurfacegroup_p.h"
#include "webossurfacegroupcompositor.h"
#include "webossurfacegroupcompositor_p.h"

#include <QtWaylandClient/private/qwaylanddisplay_p.h>
#include <QtWaylandClient/private/qwaylandwindow_p.h>

#include <QWindow>

WebOSSurfaceGroupCompositorPrivate::WebOSSurfaceGroupCompositorPrivate(struct wl_registry* registry, uint32_t id)
    : QtWayland::wl_webos_surface_group_compositor(registry, id, 1)
{

}

WebOSSurfaceGroup* WebOSSurfaceGroupCompositorPrivate::createGroup(QWaylandWindow* window, const QString &name)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
     struct ::wl_webos_surface_group* grp = create_surface_group(window->wlSurface(), name);
#else
     struct ::wl_webos_surface_group* grp = create_surface_group(window->object(), name);
#endif
     WebOSSurfaceGroup* group = new WebOSSurfaceGroup;
     WebOSSurfaceGroupPrivate* p_group = WebOSSurfaceGroupPrivate::get(group);
     p_group->init(grp);
     return group;
}

WebOSSurfaceGroup* WebOSSurfaceGroupCompositorPrivate::getGroup(const QString &name)
{
     struct ::wl_webos_surface_group* grp = get_surface_group(name);
     WebOSSurfaceGroup* group = new WebOSSurfaceGroup;
     WebOSSurfaceGroupPrivate* p_group = WebOSSurfaceGroupPrivate::get(group);
     p_group->init(grp);
     return group;
}


WebOSSurfaceGroupCompositor::WebOSSurfaceGroupCompositor(QWaylandDisplay* display, uint32_t id)
    : d_ptr(new WebOSSurfaceGroupCompositorPrivate(display->wl_registry(), id))
{
}

WebOSSurfaceGroupCompositor::~WebOSSurfaceGroupCompositor()
{
}

WebOSSurfaceGroup* WebOSSurfaceGroupCompositor::createGroup(QWindow* window, const QString &name)
{
    Q_D(WebOSSurfaceGroupCompositor);
    if (!window || !window->handle())
        return NULL;
    return d->createGroup(static_cast<QWaylandWindow*>(window->handle()), name);
}

WebOSSurfaceGroup* WebOSSurfaceGroupCompositor::getGroup(const QString &name)
{
    Q_D(WebOSSurfaceGroupCompositor);
    return d->getGroup(name);
}
