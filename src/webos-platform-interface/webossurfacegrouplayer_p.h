// Copyright (c) 2013-2018 LG Electronics, Inc.
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

#ifndef WEBOSSURFACEGROUPLAYER_P_H
#define WEBOSSURFACEGROUPLAYER_P_H

#include "qwayland-webos-surface-group.h"
#include "webossurfacegrouplayer.h"

#include <QObject>

class WebOSSurfaceGroupLayerPrivate : public QObject, public QtWayland::wl_webos_surface_group_layer {

    Q_OBJECT

    class WebOSSurfaceGroupLayer* q_ptr;
    Q_DECLARE_PUBLIC(WebOSSurfaceGroupLayer);

public:
    WebOSSurfaceGroupLayerPrivate(WebOSSurfaceGroupLayer *parent);
    ~WebOSSurfaceGroupLayerPrivate();

    void setZ(int z);

    static inline WebOSSurfaceGroupLayerPrivate* get(WebOSSurfaceGroupLayer* layer) {
        return layer->d_func();
    }
    void setKeyIndex(int keyIndex);

protected:
    virtual void webos_surface_group_layer_surface_attached();
    virtual void webos_surface_group_layer_surface_detached();
};

#endif
