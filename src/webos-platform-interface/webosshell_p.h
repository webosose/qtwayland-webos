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

#ifndef WEBOSSHELL_P_H
#define WEBOSSHELL_P_H

#include <QObject>
#include <QWindow>

#include <wayland-client.h>
#include <wayland-webos-shell-client-protocol.h>

class WebOSShellSurface;
class QWaylandDisplay;
class QWaylandShellSurface;
class QPlatformWindow;
class QWindow;

class WebOSShellPrivate {

public:

    WebOSShellPrivate(QWaylandDisplay* display, uint32_t id);
    virtual ~WebOSShellPrivate();

    static inline WebOSShellPrivate* get(WebOSShell* shell) {
        return shell->d_func();
    }

    QWaylandShellSurface* createShellSurface(QPlatformWindow* window);

    wl_webos_shell* m_shell;
    QWaylandDisplay* m_display;
};


#endif
