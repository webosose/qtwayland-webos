// Copyright (c) 2018-2019 LG Electronics, Inc.
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

#ifndef WEBOSFOREIGN_H
#define WEBOSFOREIGN_H

#include <QObject>

namespace QtWaylandClient {
    class QWaylandDisplay;
}
using QtWaylandClient::QWaylandDisplay;

class QWindow;
class QRegion;

class WebOSExported;
class WebOSImported;
class WebOSForeignPrivate;
class WebOSExportedPrivate;
class WebOSImportedPrivate;

class WebOSForeign : public QObject {
    Q_OBJECT
public:

    enum ExportedType {
        VideoObject = 0, // Exported object is Video
        SubtitleObject = 1 // Exported object is Subtitle
    };

    ~WebOSForeign();
    WebOSExported* export_element(QWindow* window, ExportedType exportedType);
    WebOSImported* import_element(const QString& windowId,
                                  ExportedType exportedType);

private:
    WebOSForeign(QWaylandDisplay* disp, uint32_t id);

    QScopedPointer<WebOSForeignPrivate> d_ptr;
    Q_DECLARE_PRIVATE(WebOSForeign);
    Q_DISABLE_COPY(WebOSForeign);

    friend class WebOSPlatformPrivate;
};

class WebOSExported : public QObject {
    Q_OBJECT
public:
    ~WebOSExported();

    void setExportedWindow(const QRegion &sourceRegion, const QRegion &destinationRegion);
    QString getWindowId();
    WebOSForeign::ExportedType getExportedType();

signals:
    void window_id_assigned(QString windowId, WebOSForeign::ExportedType type);

private:
    WebOSExported(QWindow* window);

    QScopedPointer<WebOSExportedPrivate> d_ptr;
    Q_DECLARE_PRIVATE(WebOSExported);
    Q_DISABLE_COPY(WebOSExported);

    friend class WebOSForeignPrivate;
};

class WebOSImported : public QObject {
    Q_OBJECT
public:
    ~WebOSImported();
    void requestPunchThrough();

private:
    WebOSImported(const QString& windowId,
                  WebOSForeign::ExportedType exportedType);

    QScopedPointer<WebOSImportedPrivate> d_ptr;
    Q_DECLARE_PRIVATE(WebOSImported);
    Q_DISABLE_COPY(WebOSImported);

    friend class WebOSForeignPrivate;
};

#endif
