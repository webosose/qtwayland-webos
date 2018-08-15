// Copyright (c) 2013-2019 LG Electronics, Inc.
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

#ifndef WEBOSSHELL_H
#define WEBOSSHELL_H

#include <QObject>
#include <QWindow>

class WebOSShellPrivate;
class WebOSShellSurface;

namespace QtWaylandClient {
    class QWaylandDisplay;
}
using QtWaylandClient::QWaylandDisplay;

//TODO Fix this
typedef uint uint32_t;

class WebOSShell : public QObject {

    Q_OBJECT

public:
    ~WebOSShell();

    /*!
     * Returns a shell surface handle this window. The given window must have
     * been set visible atleast once in order to have a shell surface.
     *
     * The ownership of the handle is retained and the client does MUST NOT
     * delete it!
     */
     WebOSShellSurface* shellSurfaceFor(QWindow* window);

private:
    QScopedPointer<WebOSShellPrivate> d_ptr;
    Q_DISABLE_COPY(WebOSShell);
    Q_DECLARE_PRIVATE(WebOSShell)

    WebOSShell(QWaylandDisplay* disp, uint32_t id);
    friend class WebOSShellIntegration;
    friend class WebOSPlatformPrivate;
};

#endif
