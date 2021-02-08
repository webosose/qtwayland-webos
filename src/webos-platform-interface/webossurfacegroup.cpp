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
#include "webossurfacegrouplayer.h"
#include "webossurfacegrouplayer_p.h"

#include <QWindow>
#include <QtWaylandClient/private/qwaylandwindow_p.h>
#include <QDebug>
#include <QMutableListIterator>

WebOSSurfaceGroupPrivate::WebOSSurfaceGroupPrivate()
    : QtWayland::wl_webos_surface_group()
    , q_ptr(0)
{

}

WebOSSurfaceGroupPrivate::~WebOSSurfaceGroupPrivate()
{
    wl_webos_surface_group_destroy(object());
}

void WebOSSurfaceGroupPrivate::setAllowAnonymousLayers(bool allow)
{
    allow_anonymous_layers(allow);
}

void WebOSSurfaceGroupPrivate::attachAnonymousSurface(QWaylandWindow* surface, WebOSSurfaceGroup::ZHint hint)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    attach_anonymous(surface->wlSurface(), (uint32_t)hint);
#else
    attach_anonymous(surface->object(), (uint32_t)hint);
#endif
}

WebOSSurfaceGroupLayer* WebOSSurfaceGroupPrivate::createLayer(const QString& name, int z)
{
     struct ::wl_webos_surface_group_layer* layer = create_layer(name, z);
     WebOSSurfaceGroupLayer* l = new WebOSSurfaceGroupLayer;
     WebOSSurfaceGroupLayerPrivate* p_layer = WebOSSurfaceGroupLayerPrivate::get(l);
     p_layer->init(layer);
     l->setName(name);
     l->setZ(z);
     return l;
}

void WebOSSurfaceGroupPrivate::webos_surface_group_owner_destroyed()
{
    Q_Q(WebOSSurfaceGroup);
    emit q->ownerDestroyed();

    QMutableListIterator<QPointer<QWaylandWindow> > s(m_attachedSurfaces);
    while (s.hasNext()) {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        detach(s.next().data()->wlSurface());
#else
        detach(s.next().data()->object());
#endif
        s.remove();
    }

}


void WebOSSurfaceGroupPrivate::attachSurface(QWaylandWindow* surface, const QString& layer)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    attach(surface->wlSurface(), layer);
#else
    attach(surface->object(), layer);
#endif
    m_attachedSurfaces << surface;
}

void WebOSSurfaceGroupPrivate::detachSurface(QWaylandWindow* surface)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    detach(surface->wlSurface());
#else
    detach(surface->object());
#endif
    m_attachedSurfaces.removeAll(surface);
}

void WebOSSurfaceGroupPrivate::focusOwner()
{
    focus_owner();
}

void WebOSSurfaceGroupPrivate::focusLayer(const QString& layerName)
{
    if (!layerName.isEmpty()) {
        focus_layer(layerName);
    }
}

void WebOSSurfaceGroupPrivate::commitKeyIndex(bool commit)
{
    commit_key_index(commit);
}

// Public class
WebOSSurfaceGroup::WebOSSurfaceGroup()
    : d_ptr(new WebOSSurfaceGroupPrivate)

{
    Q_D(WebOSSurfaceGroup);
    d->q_ptr = this;
}

WebOSSurfaceGroup::~WebOSSurfaceGroup()
{
}

void WebOSSurfaceGroup::setAllowAnonymousLayers(bool allow)
{
    Q_D(WebOSSurfaceGroup);
    d->setAllowAnonymousLayers(allow);
}


void WebOSSurfaceGroup::attachAnonymousSurface(QWindow* surface, ZHint hint)
{
    Q_D(WebOSSurfaceGroup);
    d->attachAnonymousSurface(static_cast<QWaylandWindow*>(surface->handle()), hint);
}

WebOSSurfaceGroupLayer* WebOSSurfaceGroup::createNamedLayer(const QString& name, int z)
{
    Q_D(WebOSSurfaceGroup);
    return d->createLayer(name, z);
}

void WebOSSurfaceGroup::attachSurface(QWindow* surface, const QString& layer)
{
    Q_D(WebOSSurfaceGroup);
    d->attachSurface(static_cast<QWaylandWindow*>(surface->handle()), layer);
}

void WebOSSurfaceGroup::detachSurface(QWindow* surface)
{
    Q_D(WebOSSurfaceGroup);
    d->detachSurface(static_cast<QWaylandWindow*>(surface->handle()));
}

void WebOSSurfaceGroup::focusOwner()
{
    Q_D(WebOSSurfaceGroup);
    d->focusOwner();
}

void WebOSSurfaceGroup::focusLayer(const QString& layerName)
{
    Q_D(WebOSSurfaceGroup);
    d->focusLayer(layerName);
}

void WebOSSurfaceGroup::commitKeyIndex(bool commit)
{
    Q_D(WebOSSurfaceGroup);
    d->commitKeyIndex(commit);
}
