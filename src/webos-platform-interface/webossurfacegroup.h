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

#ifndef WEBOSSURFACEGROUP_H
#define WEBOSSURFACEGROUP_H

#include <QObject>

class WebOSSurfaceGroupPrivate;
class QWindow;
class WebOSSurfaceGroupLayer;

class WebOSSurfaceGroup : public QObject {

    Q_OBJECT

public:

    enum ZHint {
        ZHintBelow = 0,
        ZHintAbove = 1,
        ZHintTop   = 2,
    };
    Q_DECLARE_FLAGS(ZHints, ZHint)


    WebOSSurfaceGroup();
    ~WebOSSurfaceGroup();

    void setAllowAnonymousLayers(bool allow);
    void attachAnonymousSurface(QWindow* surface, ZHint hint = ZHintAbove);
    WebOSSurfaceGroupLayer* createNamedLayer(const QString& name, int z);
    void attachSurface(QWindow* surface, const QString& layer);
    void detachSurface(QWindow* surface);

    void focusOwner();
    void focusLayer(const QString& layerName);

    void commitKeyIndex(bool commit);

signals:
    void ownerDestroyed();

private:
    QScopedPointer<WebOSSurfaceGroupPrivate> d_ptr;
    Q_DISABLE_COPY(WebOSSurfaceGroup);
    Q_DECLARE_PRIVATE(WebOSSurfaceGroup);

};

Q_DECLARE_OPERATORS_FOR_FLAGS(WebOSSurfaceGroup::ZHints)
#endif
