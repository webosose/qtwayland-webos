// Copyright (c) 2015-2021 LG Electronics, Inc.
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

#include <QtCore/qglobal.h>
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QtWaylandEglClientHwIntegration/private/qwaylandeglwindow_p.h>
#else
#include "qwaylandeglwindow.h"
#endif

#include <QtWaylandClient/private/qwaylandcursor_p.h>

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
using QtWaylandClient::QWaylandDisplay;
#endif
using QtWaylandClient::QWaylandCursor;
using QtWaylandClient::QWaylandEglWindow;
using QtWaylandClient::QWaylandInputDevice;

class WebOSShellSurface;

class WebOSPlatformWindow : public QWaylandEglWindow
{
    Q_OBJECT
public:
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    WebOSPlatformWindow(QWindow *window, QWaylandDisplay *display);
#else
    WebOSPlatformWindow(QWindow *window);
#endif

#if (QT_VERSION < QT_VERSION_CHECK(5,10,0))
    void setWindowState(Qt::WindowState state) override;
#else
    void setWindowState(Qt::WindowStates state) override;
#endif

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    void setVisible(bool visible) override;
#endif
    void setGeometry(const QRect &rect) override;

    // Global position notified by the compositor
    QPointF position() const { return m_position; }

    qreal devicePixelRatio() const override;

signals:
    void resizeRequested(const QSize &oldSize, const QSize &newSize);
    void positionChanged(const QPointF &position);

protected:
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    void doHandleFrameCallback() override;
    QRect defaultGeometry() const override;
#endif

private:
#if (QT_VERSION < QT_VERSION_CHECK(5,10,0))
    bool setWindowStateInternal(Qt::WindowState state);
#else
    bool setWindowStateInternal(Qt::WindowStates state);
#endif

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    void handleMouseLeave(QWaylandInputDevice *inputDevice) override;
    void restoreMouseCursor(QWaylandInputDevice *device) override;
#endif

private slots:
    void onShellSurfaceCreated(WebOSShellSurface *shellSurface, QPlatformWindow *window);
    void onOutputTransformChanged();
    void onDevicePixelRatioChanged();
    void onScreenChanged(QScreen *screen);

private:
    WebOSShellSurface *m_shellSurface = nullptr;

    bool m_autoOrientation;
    QRect m_initialGeometry;

#if (QT_VERSION < QT_VERSION_CHECK(5,10,0))
    Qt::WindowState mState;
#else
    Qt::WindowStates mState;
#endif

    QPointF m_position;
};

#endif
