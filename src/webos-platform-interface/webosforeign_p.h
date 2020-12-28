// Copyright (c) 2018-2021 LG Electronics, Inc.
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

#ifndef WEBOSFOREIGN_P_H
#define WEBOSFOREIGN_P_H

#include <QObject>
#include <QRegion>
#include <QWindow>

#include <private/qobject_p.h>
#include <qwayland-webos-foreign.h>

#include "webosforeign.h"

class WebOSForeignPrivate : public QObject, QtWayland::wl_webos_foreign
{
    Q_OBJECT
public:
    using QtWayland::wl_webos_foreign::export_element;
    WebOSForeignPrivate(QWaylandDisplay* display, uint32_t id);
    virtual ~WebOSForeignPrivate();

    WebOSExported* export_element(QWindow* window,
                                  WebOSForeign::WebOSExportedType exportedType);

    WebOSImported* import_element(const QString& windowId,
                                  WebOSForeign::WebOSExportedType exportedType);

    WebOSForeign *q_ptr;
    Q_DECLARE_PUBLIC(WebOSForeign);

private:
    QList<WebOSExported*> m_exportedAreas;
};

class WebOSExportedPrivate : public QObject, QtWayland::wl_webos_exported
{
    Q_OBJECT
public:
    WebOSExportedPrivate(QWindow* window);
    virtual ~WebOSExportedPrivate();

    void setExportedWindow(const QRegion &sourceRegion, const QRegion &destinationRegion);
    void setCropRegion(const QRegion &originalInputRegion, const QRegion &sourceRegion, const QRegion &destinationRegion);
    void setProperty(const QString &name, const QString &value);

    QString getWindowId();
    WebOSForeign::WebOSExportedType getWebOSExportedType();

    void destroy();

    static inline WebOSExportedPrivate* get(WebOSExported* exported) {
        return exported->d_func();
    }

    WebOSExported *q_ptr;
    Q_DECLARE_PUBLIC(WebOSExported);

protected:
    virtual void webos_exported_window_id_assigned(const QString &window_id, uint32_t exported_type);

private:
    QWindow* m_window = NULL;
    QRegion m_originalRegion;
    QRegion m_sourceRegion;
    QRegion m_destinationRegion;
    QString m_windowId;
    WebOSForeign::WebOSExportedType m_exportedType = WebOSForeign::VideoObject;

    friend class WebOSForeignPrivate;
};

class WebOSImportedPrivate : public QObject, public QtWayland::wl_webos_imported
{
    Q_OBJECT
public:
    WebOSImportedPrivate(const QString& windowId,
                         WebOSForeign::WebOSExportedType exportedType);
    virtual ~WebOSImportedPrivate();

    void requestPunchThrough(const QString& contextId);
    void attachSurface(QWaylandWindow* surface);

    void destroy();

    static inline WebOSImportedPrivate* get(WebOSImported* imported) {
        return imported->d_func();
    }

    WebOSImported *q_ptr;
    Q_DECLARE_PUBLIC(WebOSImported);

private:
    QString m_windowId;
    WebOSForeign::WebOSExportedType m_exportedType;
    friend class WebOSForeignPrivate;
};

#endif
