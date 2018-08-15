// Copyright (c) 2015-2019 LG Electronics, Inc.
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

#ifndef WEBOSWINDOW_H
#define WEBOSWINDOW_H

#include "qwaylandeglwindow.h"

#include <QtWaylandClient/private/qwaylandcursor_p.h>

using QtWaylandClient::QWaylandCursor;
using QtWaylandClient::QWaylandEglWindow;
using QtWaylandClient::QWaylandInputDevice;

class WebOSPlatformWindow : public QWaylandEglWindow
{
public:
    WebOSPlatformWindow(QWindow *window);
    void setWindowState(Qt::WindowState state) Q_DECL_OVERRIDE;
    void setGeometry(const QRect &rect) Q_DECL_OVERRIDE;

private:
    bool setWindowStateInternal(Qt::WindowState state);
    void configure(uint32_t edges, int32_t width, int32_t height);

    void handleMouseLeave(QWaylandInputDevice *inputDevice) Q_DECL_OVERRIDE;
    void restoreMouseCursor(QWaylandInputDevice *device) Q_DECL_OVERRIDE;

private slots:
    void onPositionChanged();
    void onOutputTransformChanged(const int& oldTransform, const int& newTransform);

private:
    bool m_autoOrientation;
    QRect m_initialGeometry;
};

#endif
