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

#include <QGuiApplication>
#include <qpa/qwindowsysteminterface.h>
#include <qpa/qplatformnativeinterface.h>
#include <QDebug>

#include <QtWaylandClient/private/qwaylanddisplay_p.h>
#include <QtWaylandClient/private/qwaylandwindow_p.h>

#include "webosforeign.h"
#include "webosforeign_p.h"
#include "securecoding.h"

WebOSForeignPrivate::WebOSForeignPrivate(QWaylandDisplay* display,
                                         uint32_t id)
    : QtWayland::wl_webos_foreign(display->wl_registry(), uint2int(id), 1)
    , q_ptr(0)
{
    qWarning() << "[Client:WebOSForeignPrivate] constructed:" << this;
}

WebOSForeignPrivate::~WebOSForeignPrivate()
{
    q_ptr = NULL;
    qWarning() << "[Client:WebOSForeignPrivate] destructed:" << this;
}

WebOSExported* WebOSForeignPrivate::export_element(QWindow* window, WebOSForeign::WebOSExportedType exportedType)
{
    if (!isInitialized())
        return NULL;

    if (!window) {
        qWarning() << "[Client:WebOSForeignPrivate] error: invalid window" << this;
        return NULL;
    }

    QWaylandWindow* qww = static_cast<QWaylandWindow*>(window->handle());
    if (!qww)
        return NULL;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    struct ::wl_webos_exported* wlExported = QtWayland::wl_webos_foreign::export_element(qww->wlSurface(), exportedType);
#else
    struct ::wl_webos_exported* wlExported = QtWayland::wl_webos_foreign::export_element(qww->object(), exportedType);
#endif
    if (!wlExported)
        return NULL;
    WebOSExported* exported = new WebOSExported(window);
    WebOSExportedPrivate* exported_p = WebOSExportedPrivate::get(exported);
    exported_p->init(wlExported);

    qWarning() << "[Client:WebOSForeignPrivate] requested export_element("
            << window->winId() << ","
            << exportedType << ")"
            << this;

    return exported;
}

WebOSImported* WebOSForeignPrivate::import_element(const QString& windowId,
                          WebOSForeign::WebOSExportedType exportedType)
{
    if (!isInitialized())
        return NULL;

    struct ::wl_webos_imported* wlImported = QtWayland::wl_webos_foreign::import_element(windowId, exportedType);
    WebOSImported* imported = new WebOSImported(windowId, exportedType);
    WebOSImportedPrivate* imported_p = WebOSImportedPrivate::get(imported);
    imported_p->init(wlImported);

    qWarning() << "[Client:WebOSForeignPrivate] requested import_element("
            << windowId << ","
            << exportedType << ")"
            << this;

    return imported;
}

WebOSForeign::WebOSForeign(QWaylandDisplay* display, uint32_t id)
    : d_ptr(new WebOSForeignPrivate(display, id))
{
    Q_D(WebOSForeign);
    d->q_ptr = this;

    qWarning() << "[Client:WebOSForeign] constructed:" << this;
}

WebOSForeign::~WebOSForeign()
{
    qWarning() << "[Client:WebOSForeign] destructed:" << this;
}

WebOSExported* WebOSForeign::export_element(QWindow* window, WebOSExportedType exportedType)
{
    Q_D(WebOSForeign);
    return d->export_element(window, exportedType);
}

WebOSImported* WebOSForeign::import_element(const QString& windowId,
                                            WebOSExportedType exportedType)
{
    Q_D(WebOSForeign);
    return d->import_element(windowId, exportedType);
}

WebOSExportedPrivate::WebOSExportedPrivate(QWindow* window)
    : q_ptr(0)
    , m_window(window)
{
    qWarning() << "[Client:WebOSExportedPrivate] constructed:" << this;
}

WebOSExportedPrivate::~WebOSExportedPrivate()
{
    qWarning() << "[Client:WebOSExportedPrivate] destructed:" << this;
    destroy();
    q_ptr = NULL;
}

void WebOSExportedPrivate::setExportedWindow(const QRegion &sourceRegion, const QRegion &destinationRegion)
{
    if (!isInitialized())
        return;

    m_sourceRegion = sourceRegion;
    m_destinationRegion = destinationRegion;

    QPlatformNativeInterface *wliface = QGuiApplication::platformNativeInterface();
    if (!wliface)
        return;
    wl_compositor *wlcompositor = static_cast<wl_compositor *>(wliface->nativeResourceForIntegration("compositor"));
    qreal dpr = m_window->devicePixelRatio();

    wl_region* wl_source_region = wl_compositor_create_region(wlcompositor);
    for (auto &sourceRect: m_sourceRegion) {
        wl_region_add(wl_source_region, sourceRect.x()*dpr, sourceRect.y()*dpr, sourceRect.width()*dpr, sourceRect.height()*dpr);
    }

    wl_region* wl_destination_region = wl_compositor_create_region(wlcompositor);
    for (auto &destinationRect: m_destinationRegion) {
        wl_region_add(wl_destination_region, destinationRect.x()*dpr, destinationRect.y()*dpr, destinationRect.width()*dpr, destinationRect.height()*dpr);
    }

    set_exported_window(wl_source_region, wl_destination_region);

    wl_region_destroy(wl_source_region);
    wl_region_destroy(wl_destination_region);
}

void WebOSExportedPrivate::setCropRegion(const QRegion &originalInputRegion, const QRegion &sourceRegion, const QRegion &destinationRegion)
{
    if (!isInitialized())
        return;

    m_originalRegion = originalInputRegion;
    m_sourceRegion = sourceRegion;
    m_destinationRegion = destinationRegion;

    QPlatformNativeInterface *wliface = QGuiApplication::platformNativeInterface();
    if (!wliface)
        return;
    wl_compositor *wlcompositor = static_cast<wl_compositor *>(wliface->nativeResourceForIntegration("compositor"));
    qreal dpr = m_window->devicePixelRatio();

    wl_region* wl_original_region = wl_compositor_create_region(wlcompositor);
    for (auto &originalRect: m_originalRegion) {
        wl_region_add(wl_original_region, originalRect.x()*dpr, originalRect.y()*dpr, originalRect.width()*dpr, originalRect.height()*dpr);
    }

    wl_region* wl_source_region = wl_compositor_create_region(wlcompositor);
    for (auto &sourceRect: m_sourceRegion) {
        wl_region_add(wl_source_region, sourceRect.x()*dpr, sourceRect.y()*dpr, sourceRect.width()*dpr, sourceRect.height()*dpr);
    }

    wl_region* wl_destination_region = wl_compositor_create_region(wlcompositor);
    for (auto &destinationRect: m_destinationRegion) {
        wl_region_add(wl_destination_region, destinationRect.x()*dpr, destinationRect.y()*dpr, destinationRect.width()*dpr, destinationRect.height()*dpr);
    }

    set_crop_region(wl_original_region, wl_source_region, wl_destination_region);

    wl_region_destroy(wl_original_region);
    wl_region_destroy(wl_source_region);
    wl_region_destroy(wl_destination_region);
}

void WebOSExportedPrivate::setProperty(const QString &name, const QString &value)
{
    if (!isInitialized())
        return;

    set_property(name.toLatin1().constData(), value.toLatin1().constData());
}

QString WebOSExportedPrivate::getWindowId()
{
    return m_windowId;
}

WebOSForeign::WebOSExportedType WebOSExportedPrivate::getWebOSExportedType()
{
    return m_exportedType;
}

void WebOSExportedPrivate::destroy()
{
    if (QtWayland::wl_webos_exported::isInitialized()) {
        qWarning() << "[Client:WebOSExportedPrivate] destroy:" << this;
        QtWayland::wl_webos_exported::destroy();
    }
}

void WebOSExportedPrivate::webos_exported_window_id_assigned(const QString &window_id, uint32_t exported_type)
{
    qWarning() << "[Client:WebOSExported] window_id assigned:" << window_id;
    Q_Q(WebOSExported);
    m_windowId = window_id;
    m_exportedType = static_cast<WebOSForeign::WebOSExportedType>(exported_type);
    q->windowIdAssigned(m_windowId, m_exportedType);
}

WebOSExported::WebOSExported(QWindow *window)
    : d_ptr(new WebOSExportedPrivate(window))
{
    Q_D(WebOSExported);
    d->q_ptr = this;

    qWarning() << "[Client:WebOSExported] constructed:" << this;
}

WebOSExported::~WebOSExported()
{
    qWarning() << "[Client:WebOSExported] destructed:" << this;
}

void WebOSExported::setExportedWindow(const QRegion &sourceRegion, const QRegion &destinationRegion)
{
    Q_D(WebOSExported);
    return d->setExportedWindow(sourceRegion, destinationRegion);
}

void WebOSExported::setCropRegion(const QRegion &originalRegion, const QRegion &sourceRegion, const QRegion &destinationRegion)
{
    Q_D(WebOSExported);
    return d->setCropRegion(originalRegion, sourceRegion, destinationRegion);
}

void WebOSExported::setProperty(const QString & name, const QString & value)
{
    Q_D(WebOSExported);
    return d->setProperty(name, value);
}

QString WebOSExported::getWindowId()
{
    Q_D(WebOSExported);
    return d->getWindowId();
}

WebOSForeign::WebOSExportedType WebOSExported::getWebOSExportedType()
{
    Q_D(WebOSExported);
    return d->getWebOSExportedType();
}

void WebOSExported::destroy()
{
    Q_D(WebOSExported);
    d->destroy();
}

WebOSImportedPrivate::WebOSImportedPrivate(const QString& windowId,
                          WebOSForeign::WebOSExportedType exportedType)

    : q_ptr(0)
    , m_windowId(windowId)
    , m_exportedType(exportedType)
{
    qWarning() << "[Client:WebOSImportedPrivate] constructed:" << this;
}

WebOSImportedPrivate::~WebOSImportedPrivate()
{
    qWarning() << "[Client:WebOSImportedPrivate] destructed:" << this;
    destroy();
    q_ptr = NULL;
}

void WebOSImportedPrivate::requestPunchThrough(const QString& contextId)
{
    attach_punchthrough_with_context(contextId);
}

void WebOSImportedPrivate::setPunchThrough(const QString& contextId)
{
    set_punchthrough(contextId);
}

void WebOSImportedPrivate::attachSurface(QWaylandWindow* surface)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    attach_surface(surface->wlSurface());
#else
    attach_surface(surface->object());
#endif
}

void WebOSImportedPrivate::destroy()
{
    if (QtWayland::wl_webos_imported::isInitialized()) {
        qWarning() << "[Client:WebOSImportedPrivate] destroy:" << this;
        QtWayland::wl_webos_imported::destroy();
    }
}

WebOSImported::WebOSImported(const QString& windowId,
                             WebOSForeign::WebOSExportedType exportedType)
    : d_ptr(new WebOSImportedPrivate(windowId, exportedType))
{
    Q_D(WebOSImported);
    d->q_ptr = this;

    qWarning() << "[Client:WebOSImported] constructed:" << this;
}

WebOSImported::~WebOSImported()
{
    qWarning() << "[Client:WebOSImported] destructed:" << this;
}

void WebOSImported::requestPunchThrough(const QString& contextId)
{
    Q_D(WebOSImported);
    d->requestPunchThrough(contextId);
}

void WebOSImported::setPunchThrough(const QString& contextId)
{
    Q_D(WebOSImported);
    d->setPunchThrough(contextId);
}

void WebOSImported::attachSurface(QWindow* surface)
{
    Q_D(WebOSImported);
    if (surface && surface->handle())
        d->attachSurface(static_cast<QWaylandWindow*>(surface->handle()));
}

void WebOSImported::destroy()
{
    Q_D(WebOSImported);
    d->destroy();
}
