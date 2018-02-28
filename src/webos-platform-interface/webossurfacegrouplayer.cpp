// Copyright (c) 2014-2018 LG Electronics, Inc.
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

#include <QDebug>

#include "webossurfacegrouplayer.h"
#include "webossurfacegrouplayer_p.h"

WebOSSurfaceGroupLayerPrivate::WebOSSurfaceGroupLayerPrivate(WebOSSurfaceGroupLayer *parent)
    : QtWayland::wl_webos_surface_group_layer()
    , q_ptr(parent)
{

}

WebOSSurfaceGroupLayerPrivate::~WebOSSurfaceGroupLayerPrivate()
{
    destroy();
    q_ptr = 0;
}

void WebOSSurfaceGroupLayerPrivate::setZ(int z)
{
    set_z_index(z);
}

void WebOSSurfaceGroupLayerPrivate::webos_surface_group_layer_surface_attached()
{
    Q_Q(WebOSSurfaceGroupLayer);
    emit q->surfaceAttached();
}

void WebOSSurfaceGroupLayerPrivate::webos_surface_group_layer_surface_detached()
{
    Q_Q(WebOSSurfaceGroupLayer);
    emit q->surfaceDetached();
}

WebOSSurfaceGroupLayer::WebOSSurfaceGroupLayer()
    : d_ptr(new WebOSSurfaceGroupLayerPrivate(this))
    , m_z(0)
{
}

WebOSSurfaceGroupLayer::~WebOSSurfaceGroupLayer()
{
}

void WebOSSurfaceGroupLayer::setZ(int z)
{
    m_z = z;
    d_ptr->setZ(m_z);
}
