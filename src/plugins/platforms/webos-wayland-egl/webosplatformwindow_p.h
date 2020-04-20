// Copyright (c) 2015-2020 LG Electronics, Inc.
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

#if (QT_VERSION < QT_VERSION_CHECK(5,10,0))
    void setWindowState(Qt::WindowState state) Q_DECL_OVERRIDE;
#else
    void setWindowState(Qt::WindowStates state) Q_DECL_OVERRIDE;
#endif

    void setGeometry(const QRect &rect) Q_DECL_OVERRIDE;

private:
#if (QT_VERSION < QT_VERSION_CHECK(5,10,0))
    bool setWindowStateInternal(Qt::WindowState state);
#else
    bool setWindowStateInternal(Qt::WindowStates state);
#endif

    void handleMouseLeave(QWaylandInputDevice *inputDevice) Q_DECL_OVERRIDE;
    void restoreMouseCursor(QWaylandInputDevice *device) Q_DECL_OVERRIDE;

private slots:
    void onPositionChanged();
    void onOutputTransformChanged();
    void onDevicePixelRatioChanged();
    void onScreenChanged(QScreen *screen);

private:
    bool m_autoOrientation;
    QRect m_initialGeometry;

#if (QT_VERSION < QT_VERSION_CHECK(5,10,0))
    Qt::WindowState mState;
#else
    Qt::WindowStates mState;
#endif
};

#endif
