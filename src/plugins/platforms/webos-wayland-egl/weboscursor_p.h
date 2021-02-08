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

#ifndef WEBOSCURSOR_H
#define WEBOSCURSOR_H

#include <QtWaylandClient/private/qwaylandcursor_p.h>
#include <QtWaylandClient/private/qwaylandshmbackingstore_p.h>

using QtWaylandClient::QWaylandCursor;
using QtWaylandClient::QWaylandCursorTheme;
using QtWaylandClient::QWaylandShmBuffer;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
using QtWaylandClient::QWaylandDisplay;
#else
using QtWaylandClient::QWaylandScreen;
#endif

class WebOSCursor : public QWaylandCursor
{
public:
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    WebOSCursor(QWaylandDisplay *display);
#else
    WebOSCursor(QWaylandScreen *screen);
#endif
    ~WebOSCursor();
private:
    void changeCursor(QCursor *cursor, QWindow *window);
    void createBitmapCursor(QPixmap cursorPixmap, QPoint hotSpot);

    QWaylandShmBuffer *mCustomCursorBuffer;
};

#endif
