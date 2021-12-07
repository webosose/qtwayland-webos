// Copyright (c) 2021 LG Electronics, Inc.
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

#ifndef WEBOSPLATFORM_H
#define WEBOSPLATFORM_H

#include <QObject>
#include <QScopedPointer>

#ifdef HAS_CRIU
#include "appsnapshotmanager.h"
#endif

class WebOSPlatformPrivate;
class WebOSShell;

class WebOSSurfaceGroupCompositor;
class WebOSSurfaceGroup;
class WebOSInputManager;
class WebOSInputPanelLocator;
class WebOSForeign;
class WebOSTablet;
class WebOSPresentationTime;

class WebOSPlatform : public QObject {

    Q_OBJECT

public:
    static WebOSPlatform* instance();

    ~WebOSPlatform();

    WebOSShell* shell();

    WebOSSurfaceGroupCompositor* surfaceGroupCompositor();

    WebOSInputManager* inputManager();

    WebOSInputPanelLocator* inputPanelLocator();

    WebOSForeign* webOSForeign();

    WebOSTablet* webOSTablet();

    WebOSPresentationTime *presentation();
#ifdef HAS_CRIU
    AppSnapshotManager* appSnapshotManager();

private slots:
    void onStateChanged(const AppSnapshotManager::AppSnapShotState state, const QString failureReason);
#endif

private:
    WebOSPlatform();

    static WebOSPlatform* m_instance;
    QScopedPointer<WebOSPlatformPrivate> d_ptr;

    Q_DECLARE_PRIVATE(WebOSPlatform)
    Q_DISABLE_COPY(WebOSPlatform)
};

#endif
