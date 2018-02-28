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

#ifndef WEBOSSURFACEGROUPCOMPOSITOR_H
#define WEBOSSURFACEGROUPCOMPOSITOR_H

#include <stdint.h>
#include <QObject>
#include <QWindow>

class QWaylandDisplay;
class QWindow;
class WebOSSurfaceGroupCompositorPrivate;
class WebOSSurfaceGroup;

class WebOSSurfaceGroupCompositor : public QObject {

    Q_OBJECT

public:
    ~WebOSSurfaceGroupCompositor();
    WebOSSurfaceGroup* createGroup(QWindow* window, const QString &name);
    WebOSSurfaceGroup* getGroup(const QString &name);

private:
    QScopedPointer<WebOSSurfaceGroupCompositorPrivate> d_ptr;
    Q_DISABLE_COPY(WebOSSurfaceGroupCompositor);
    Q_DECLARE_PRIVATE(WebOSSurfaceGroupCompositor);

    WebOSSurfaceGroupCompositor(QWaylandDisplay* disp, uint32_t id);
    friend class WebOSPlatformPrivate;
};

#endif
