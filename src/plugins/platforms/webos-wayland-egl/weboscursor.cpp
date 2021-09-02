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

#include <wayland-cursor.h>
#include <QtWaylandClient/private/qwaylanddisplay_p.h>
#include <QtGui/QPainter>
#include <QGuiApplication>
#include <QDebug>

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QtWaylandClient/private/qwaylandinputdevice_p.h>
#else
#include <QtWaylandClient/private/qwaylandscreen_p.h>
#endif

#include "weboscursor_p.h"

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
WebOSCursor::WebOSCursor(QWaylandDisplay *display)
    : QWaylandCursor(display)
    , mCustomCursorBuffer(0)
{
}
#else
WebOSCursor::WebOSCursor(QWaylandScreen *screen)
    : QWaylandCursor(screen)
    , mCustomCursorBuffer(0)
{
}
#endif

WebOSCursor::~WebOSCursor()
{
    delete mCustomCursorBuffer;
}

void WebOSCursor::changeCursor(QCursor *cursor, QWindow *window)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    const Qt::CursorShape newShape = cursor ? cursor->shape() : Qt::ArrowCursor;

    // Cursor shapes below need webOS specific handling with revserved hotspots.
    //   1) ArrowCursor: hotspot of 255
    //   2) BlankCursor: hotspot of 254
    // For other shapes, use the method of the base class.

    // NOTE:
    // To prevent QWaylandWindow::restoreMouseCursor reverts the cursor shape,
    // we have to call QWindow::setCursor in this method. Since it is supposed
    // to call QPlatformCursor::changeCursor again, it is important to make
    // the control flow go to QWaylandCursor::changeCursor in the end.

    int hs = 0;
    switch (newShape) {
    case Qt::ArrowCursor:
        hs = 255;
        break;
    case Qt::BlankCursor:
        hs = 254;
        break;
    default:
        QCursor *cp = QGuiApplication::overrideCursor();
        if (cp && cp != cursor) {
            qWarning() << "setting cursor with overrideCursor" << cp->shape();
            // Update window's cursor as otherwise it will be
            // reverted by QWaylandWindow::restoreMouseCursor().
            // It is important to check if cp != cursor to prevent
            // recursive calls.
            window->setCursor(*cp);
            return;
        } else {
            qDebug() << "setting cursor" << newShape;
            cp = cursor;
        }
        QWaylandCursor::changeCursor(cp, window);
        return;
    }

    // Continue webOS specific cursor handling.
    // As only hotspot matters in this case, use a bitmap cursor with dummy content.
    QPixmap cpix = QPixmap(255, 255);
    cpix.fill(Qt::transparent);
    QCursor newCursor = QCursor(cpix, hs, hs);

    qDebug() << "webOS specific cursor handling for:" << newShape;

    // Apply newCursor to all input devices.
    for (auto *inputDevice : mDisplay->inputDevices())
        inputDevice->setCursor(&newCursor);
    // Update window's cursor as otherwise it will be
    // reverted by QWaylandWindow::restoreMouseCursor().
    // Calling setCursor with a bitmap cursor will lead the control
    // to QWaylandCursor::changeCursor.
    window->setCursor(newCursor);
#else
    Q_UNUSED(window);

    const Qt::CursorShape newShape = cursor ? cursor->shape() : Qt::ArrowCursor;

    if (newShape > Qt::BitmapCursor) {
        qWarning() << "Unsupported cursor shape:" << newShape;
        return;
    } else if (newShape == Qt::BitmapCursor) {
        createBitmapCursor(cursor->pixmap(), cursor->hotSpot());
        return;
    }

    // webOS specific cursor handling with reserved hotspot values
    // 1) 255: ArrowCursor
    // 2) 254: BlankCursor
    switch (newShape) {
    case Qt::ArrowCursor:
    case Qt::BlankCursor: {
        QPixmap cPix = QPixmap(255, 255);
        cPix.fill(Qt::transparent);
        createBitmapCursor(cPix, newShape == Qt::ArrowCursor ? QPoint(255,255) : QPoint(254,254));
        break;
    }
    default:
        struct wl_cursor_image *image = mCursorTheme->cursorImage(newShape);
        if (image == nullptr) {
            qWarning() << "Could not get cursor";
            return;
        }
        struct wl_buffer *buffer = wl_cursor_image_get_buffer(image);
        if (!buffer) {
            qWarning("Could not find buffer for cursor");
            return;
        }
        mDisplay->setCursor(buffer, image, 1.0);
        break;
    }
#endif
}

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
void WebOSCursor::createBitmapCursor(QPixmap cursorPixmap, QPoint hotSpot)
{
    if (mCustomCursorBuffer != NULL) {
        delete mCustomCursorBuffer;
    }

    mCustomCursorBuffer = new QWaylandShmBuffer(mDisplay, cursorPixmap.size(),
                                                QImage::Format_ARGB32_Premultiplied);

    {
        QPainter p(mCustomCursorBuffer->image());
        p.drawPixmap(0,0,cursorPixmap);
    }

    struct wl_cursor_image customCursorImage = {cursorPixmap.width(), cursorPixmap.height(),
                                                hotSpot.x(), hotSpot.y(), 0};

    mDisplay->setCursor(mCustomCursorBuffer->buffer(), &customCursorImage, 1.0);
}
#endif
