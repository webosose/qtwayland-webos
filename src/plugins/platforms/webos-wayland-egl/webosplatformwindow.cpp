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

#include "webosplatformwindow_p.h"
#include "webosscreen_p.h"

#include <webosplatform.h>
#include <webosshell.h>
#include <webosshellsurface.h>
#include <webosshellsurface_p.h>

#include <QtWaylandClient/private/qwaylandshellsurface_p.h>

#include <QGuiApplication>
#include <QDebug>
#include <qpa/qwindowsysteminterface.h>

static WebOSShellSurface *
webOSShellSurfaceFor(QWindow *window)
{
    WebOSPlatform *platform = WebOSPlatform::instance();
    WebOSShell *shell = platform ? platform->shell() : NULL;
    return shell ? shell->shellSurfaceFor(window) : NULL;
}

WebOSPlatformWindow::WebOSPlatformWindow(QWindow *window)
    : QWaylandEglWindow(window)
    , m_autoOrientation(true)
    , mState(Qt::WindowNoState)
{
    setWindowStateInternal(window->windowState());

    // If a client sets WEBOS_WINDOW_NO_AUTO_ORIENTATION to 1, it is assumed that
    // the client wants to change its own geometry by itself
    if (qgetenv("WEBOS_WINDOW_NO_AUTO_ORIENTATION").toInt() == 1)
        m_autoOrientation = false;

    WebOSShellSurfacePrivate* ssp = static_cast<WebOSShellSurfacePrivate *>(shellSurface());
    if (ssp) {
        WebOSShellSurface* ss = ssp->shellSurface();
        QObject::connect(ss, &WebOSShellSurface::positionChanged, this, &WebOSPlatformWindow::onPositionChanged);
    }

    WebOSScreen *screen = static_cast<WebOSScreen *>(waylandScreen());
    QObject::connect(screen, &WebOSScreen::outputTransformChanged, this, &WebOSPlatformWindow::onOutputTransformChanged);
    QObject::connect(screen, &WebOSScreen::devicePixelRatioChanged, this, &WebOSPlatformWindow::onDevicePixelRatioChanged);
}

#if (QT_VERSION < QT_VERSION_CHECK(5,10,0))
bool WebOSPlatformWindow::setWindowStateInternal(Qt::WindowState state)
#else
bool WebOSPlatformWindow::setWindowStateInternal(Qt::WindowStates state)
#endif
{
    if (mState == state) {
        return false;
    }

    // As of february 2013 QWindow::setWindowState sets the new state value after
    // QPlatformWindow::setWindowState returns, so we cannot rely on QWindow::windowState
    // here. We use then this mState variable.
    mState = state;

    /* TODO: Those functions are protected, so we will do more implementation to call it
     * when it is really needed.
    if (mShellSurface) {
        switch (state) {
            case Qt::WindowFullScreen:
                mShellSurface->setFullscreen();
                break;
            case Qt::WindowMaximized:
                mShellSurface->setMaximized();
                break;
            case Qt::WindowMinimized:
                mShellSurface->setMinimized();
                break;
            default:
                mShellSurface->setNormal();
        }
    }
    */

    // Not to call it to wait state_changed from compositor
    //QWindowSystemInterface::handleWindowStateChanged(window(), mState);

    return true;
}

#if (QT_VERSION < QT_VERSION_CHECK(5,10,0))
void WebOSPlatformWindow::setWindowState(Qt::WindowState state)
#else
void WebOSPlatformWindow::setWindowState(Qt::WindowStates state)
#endif
{
    setWindowStateInternal(state);

    WebOSShellSurface *ss = webOSShellSurfaceFor(window());
    if (ss)
        ss->setState(state);
}

void WebOSPlatformWindow::setGeometry(const QRect &rect)
{
    bool initialize = false;
    if (m_initialGeometry.size().isEmpty() && !rect.size().isEmpty()) {
        initialize = true;
        m_initialGeometry = rect;
    }

    QWaylandEglWindow::setGeometry(rect);
    WebOSShellSurface *ss = webOSShellSurfaceFor(window());
    if (ss)
        ss->setInputRegion(QRect(0, 0, geometry().width(), geometry().height()));

    // Handle transform for initial geometry
    if (initialize) {
        WebOSScreen *screen = static_cast<WebOSScreen *>(waylandScreen());
        if (WebOSScreen::compareOutputTransform(0, screen->currentTransform()))
            onOutputTransformChanged();
    }
}

void WebOSPlatformWindow::onPositionChanged()
{
    WebOSShellSurface *ss = webOSShellSurfaceFor(window());

    Q_ASSERT(sender() == ss);
    const QPointF &pos = ss->position();
    setGeometry(QRect(pos.x(), pos.y(),
                geometry().width(), geometry().height()));
}

void WebOSPlatformWindow::onOutputTransformChanged()
{
    if (m_autoOrientation) {
        WebOSScreen *screen = static_cast<WebOSScreen *>(waylandScreen());
        bool isOutputPortrait = screen->geometry().height() > screen->geometry().width();
        bool isWindowPortrait = geometry().height() > geometry().width();

        if (isOutputPortrait != isWindowPortrait) {
            // Swap width and height to make window and output orientation in sync
            QRect newGeometry(geometry());
            newGeometry.setWidth(geometry().height());
            newGeometry.setHeight(geometry().width());
            qInfo() << "Update platform window geometry as per screen geometry change:" << geometry() << "->" << newGeometry;
            setGeometry(newGeometry);
        } else {
            qInfo() << "Keep the platform window geometry:" << geometry() << "screen:" << screen->geometry();
        }
    } else {
        qInfo() << "No platform window geometry change as WEBOS_WINDOW_NO_AUTO_ORIENTATION is set";
    }
}

void WebOSPlatformWindow::handleMouseLeave(QWaylandInputDevice *inputDevice)
{
    if (mWindowDecoration) {
        if (mMouseEventsInContentArea)
            QWindowSystemInterface::handleLeaveEvent(window());
    } else {
        QWindowSystemInterface::handleLeaveEvent(window());
    }
}

void WebOSPlatformWindow::restoreMouseCursor(QWaylandInputDevice *device)
{
    //When App set overrideCursor apply it first.
    QCursor *cp = QGuiApplication::overrideCursor();
    QCursor c;
    if (!cp) {
        c = window()->cursor();
        cp = &c;
    }

    //Do not use qt's setCursor here. Cause App's window cursor haven't chagned,
    //it will not affect current cursor shape, that is same shape.
    waylandScreen()->waylandCursor()->changeCursor(cp, window());
}

void WebOSPlatformWindow::onDevicePixelRatioChanged()
{
    updateSurface(false);
}
