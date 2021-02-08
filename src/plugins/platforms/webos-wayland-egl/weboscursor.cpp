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
#include <QDebug>

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
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
    Q_UNUSED(window)

    const Qt::CursorShape newShape = cursor ? cursor->shape() : Qt::ArrowCursor;

    if (newShape > Qt::BitmapCursor) {
        qWarning() << "Unsupported cursor shape:" << newShape;
        return;
    } else if (newShape == Qt::BitmapCursor) {
        createBitmapCursor(cursor->pixmap(), cursor->hotSpot());
        return;
    }

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    struct wl_cursor_image *image = nullptr;
#else
    struct wl_cursor_image *image = mCursorTheme->cursorImage(newShape);
#endif

    if (image == nullptr) {
        qWarning() << "Could not get cursor";
        return;
    }

    // webOS specific cursor handling with reserved hotspot values
    // 1) 255: ArrowCursor
    // 2) 254: BlankCursor
    if (newShape == Qt::ArrowCursor) {
        image->hotspot_x = 255;
        image->hotspot_y = 255;
    } else if (newShape == Qt::BlankCursor) {
        image->hotspot_x = 254;
        image->hotspot_y = 254;
    } else {
        // TODO
    }

    struct wl_buffer *buffer = wl_cursor_image_get_buffer(image);

    if (!buffer) {
        qWarning("Could not find buffer for cursor");
        return;
    }

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    mDisplay->setCursor(buffer, image, 1.0);
#endif
}

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

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    mDisplay->setCursor(mCustomCursorBuffer->buffer(), &customCursorImage, 1.0);
#endif
}
