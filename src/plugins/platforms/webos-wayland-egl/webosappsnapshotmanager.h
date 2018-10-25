// Copyright (c) 2017-2018 LG Electronics, Inc.
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


#ifndef WEBOSAPPSNAPSHOTMANAGER_H
#define WEBOSAPPSNAPSHOTMANAGER_H

#include "appsnapshotmanager.h"

class QWaylandDisplay;
class QWaylandEglClientBufferIntegration;
class WebOSAppSnapshotManagerPrivate;

class WebOSAppSnapshotManager : public AppSnapshotManager
{
    Q_OBJECT
public:
    WebOSAppSnapshotManager(QWaylandDisplay* dpy, QWaylandEglClientBufferIntegration* cbi);

private slots:
    void onAboutToBlock();

private:
    Q_DECLARE_PRIVATE(WebOSAppSnapshotManager)
    Q_DISABLE_COPY(WebOSAppSnapshotManager)
};

#endif
