// Copyright (c) 2015-2018 LG Electronics, Inc.
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

#include "weboscursor_p.h"

WebOSCursor::WebOSCursor(QWaylandScreen *screen)
    : QWaylandCursor(screen)
    , mCustomCursorBuffer(0)
{

}

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

    struct wl_cursor *waylandCursor = requestCursor((WaylandCursor)newShape);

    // webOS specific cursor handling with reserved hotspot values
    // 1) 255: ArrowCursor
    // 2) 254: BlankCursor
    if (newShape == Qt::ArrowCursor) {
        waylandCursor->images[0]->hotspot_x = 255;
        waylandCursor->images[0]->hotspot_y = 255;
    } else if (newShape == Qt::BlankCursor) {
        waylandCursor->images[0]->hotspot_x = 254;
        waylandCursor->images[0]->hotspot_y = 254;
    } else {
        // TODO
    }

    struct wl_cursor_image *image = waylandCursor->images[0];
    struct wl_buffer *buffer = wl_cursor_image_get_buffer(image);

    if (!buffer) {
        qWarning("Could not find buffer for cursor");
        return;
    }

    mDisplay->setCursor(buffer, image);
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

    mDisplay->setCursor(mCustomCursorBuffer->buffer(), &customCursorImage);
}
