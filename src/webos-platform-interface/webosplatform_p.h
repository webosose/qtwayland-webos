// Copyright (c) 2013-2021 LG Electronics, Inc.
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

#ifndef WEBOSPLATFORM_P_H
#define WEBOSPLATFORM_P_H

#include <QtWaylandClient/private/qwaylanddisplay_p.h>

class WebOSShell;
class WebOSSurfaceGroupCompositor;
class WebOSInputManager;
class WebOSForeign;
class WebOSTablet;
#ifdef HAS_CRIU
class AppSnapshotManager;
#endif
class WebOSPresentationTime;

using QtWaylandClient::QWaylandDisplay;
using QtWaylandClient::QWaylandIntegration;

class WebOSPlatformPrivate {

public:
    WebOSPlatformPrivate();
    ~WebOSPlatformPrivate();

    static inline WebOSPlatformPrivate* get(WebOSPlatform* p) {
        return p->d_func();
    }
    void setShell(WebOSShell* shell);
    QWaylandDisplay *display() { return m_display; }
    WebOSPresentationTime *presentation() const { return mPresentation; }

    static void registry_global(void *data, struct wl_registry *registry, uint32_t id, const QString &interface, uint32_t version);

    WebOSShell* m_shell;
    WebOSSurfaceGroupCompositor* m_groupCompositor;
    WebOSInputManager* m_inputManager;
    WebOSForeign* m_foreign;
    WebOSTablet* m_webosTablet = nullptr;
    QWaylandDisplay *m_display;
#ifdef HAS_CRIU
    AppSnapshotManager* m_appSnapshotManager;
#endif
    WebOSPresentationTime *mPresentation = nullptr;
};

#endif
