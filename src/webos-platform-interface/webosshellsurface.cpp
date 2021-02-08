// Copyright (c) 2013-2021 LG Electronics, Inc.
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

#include "webosshellsurface.h"
#include "webosshellsurface_p.h"

#include <QtWaylandClient/private/qwaylandwindow_p.h>

#include <QDebug>
#include <qpa/qwindowsysteminterface.h>
#include <qpa/qplatformnativeinterface.h>
#include <QGuiApplication>

const struct wl_webos_shell_surface_listener WebOSShellSurfacePrivate::listener = {
    WebOSShellSurfacePrivate::state_changed,
    WebOSShellSurfacePrivate::position_changed,
    WebOSShellSurfacePrivate::close,
    WebOSShellSurfacePrivate::exposed,
    WebOSShellSurfacePrivate::state_about_to_change,
    WebOSShellSurfacePrivate::addon_status_changed
};

#if (QT_VERSION < QT_VERSION_CHECK(5,10,0))
static uint32_t waylandStateFromQtWindowState(Qt::WindowState state)
{
    switch (state) {
        case Qt::WindowNoState: return WL_WEBOS_SHELL_SURFACE_STATE_DEFAULT;
        case Qt::WindowMinimized: return WL_WEBOS_SHELL_SURFACE_STATE_MINIMIZED;
        case Qt::WindowMaximized: return WL_WEBOS_SHELL_SURFACE_STATE_MAXIMIZED;
        case Qt::WindowFullScreen: return WL_WEBOS_SHELL_SURFACE_STATE_FULLSCREEN;
        default: return WL_WEBOS_SHELL_SURFACE_STATE_DEFAULT;
    }
}
#else
static uint32_t waylandStateFromQtWindowState(Qt::WindowStates state)
{
    if (state.testFlag(Qt::WindowNoState))
        return WL_WEBOS_SHELL_SURFACE_STATE_DEFAULT;
    else if (state.testFlag(Qt::WindowMinimized))
        return WL_WEBOS_SHELL_SURFACE_STATE_MINIMIZED;
    else if (state.testFlag(Qt::WindowMaximized))
        return WL_WEBOS_SHELL_SURFACE_STATE_MAXIMIZED;
    else if (state.testFlag(Qt::WindowFullScreen))
        return WL_WEBOS_SHELL_SURFACE_STATE_FULLSCREEN;
    else
        return WL_WEBOS_SHELL_SURFACE_STATE_DEFAULT;
}
#endif

static Qt::WindowState qtWindowStateFromWaylandState(uint32_t state)
{
    switch (state) {
        case WL_WEBOS_SHELL_SURFACE_STATE_DEFAULT: return Qt::WindowNoState;
        case WL_WEBOS_SHELL_SURFACE_STATE_MINIMIZED: return Qt::WindowMinimized;
        case WL_WEBOS_SHELL_SURFACE_STATE_MAXIMIZED: return Qt::WindowMaximized;
        case WL_WEBOS_SHELL_SURFACE_STATE_FULLSCREEN: return Qt::WindowFullScreen;
        default: return Qt::WindowNoState;
    }
}

static WebOSShellSurface::AddonStatus addonStatusFromWaylandState(uint32_t state)
{
    switch (state) {
    case WL_WEBOS_SHELL_SURFACE_ADDON_STATUS_NULL:   return WebOSShellSurface::AddonStatusNull;
    case WL_WEBOS_SHELL_SURFACE_ADDON_STATUS_LOADED: return WebOSShellSurface::AddonStatusLoaded;
    case WL_WEBOS_SHELL_SURFACE_ADDON_STATUS_DENIED: return WebOSShellSurface::AddonStatusDenied;
    case WL_WEBOS_SHELL_SURFACE_ADDON_STATUS_ERROR:  return WebOSShellSurface::AddonStatusError;
    default: return WebOSShellSurface::AddonStatusNull;
    }
}

WebOSShellSurfacePrivate::WebOSShellSurfacePrivate(wl_webos_shell_surface* shellSurface, struct ::wl_shell_surface *shell_surface, QPlatformWindow* parent)
    : QWaylandWlShellSurface(shell_surface, static_cast<QWaylandWindow *>(parent))
    , q_ptr(0)
    , m_shellSurface(shellSurface)
    , m_locationHint(WebOSShellSurface::LocationHintUnknown)
    , m_keyMask(WebOSShellSurface::KeyMaskDefault)
    , m_state(Qt::WindowNoState)
{
    m_parent = static_cast<QWaylandWindow*>(parent);
    wl_webos_shell_surface_add_listener(m_shellSurface, &listener, this);
}

WebOSShellSurfacePrivate::~WebOSShellSurfacePrivate()
{
    wl_webos_shell_surface_destroy(m_shellSurface);
    delete q_ptr;
    q_ptr = NULL;
}

void WebOSShellSurfacePrivate::state_changed(void *data, struct wl_webos_shell_surface *wl_webos_shell_surface, uint32_t state)
{
    Q_UNUSED(wl_webos_shell_surface);
    WebOSShellSurfacePrivate* shell = static_cast<WebOSShellSurfacePrivate*>(data);
    Qt::WindowState qtState = qtWindowStateFromWaylandState(state);

    /* Update window state for QWindow/QWaylandWindow */
    shell->m_state = qtState;
#if (QT_VERSION < QT_VERSION_CHECK(5,10,0))
    shell->m_parent->window()->setWindowState(shell->m_state);
#else
    shell->m_parent->window()->setWindowState(qtWindowStateFromWaylandState(waylandStateFromQtWindowState(shell->m_state)));
#endif

    QWindowSystemInterface::handleWindowStateChanged(shell->m_parent->window(), shell->m_state);
    QWindowSystemInterface::flushWindowSystemEvents(); // Required for oldState to work on WindowStateChanged
}

void WebOSShellSurfacePrivate::close(void *data, struct wl_webos_shell_surface *wl_webos_shell_surface)
{
    qDebug() << __PRETTY_FUNCTION__;
    Q_UNUSED(wl_webos_shell_surface);
    WebOSShellSurfacePrivate* shell = static_cast<WebOSShellSurfacePrivate*>(data);
    QWindowSystemInterface::handleCloseEvent(shell->m_parent->window());
}

void WebOSShellSurfacePrivate::position_changed(void *data, struct wl_webos_shell_surface *wl_webos_shell_surface, int32_t x, int32_t y)
{
    Q_UNUSED(wl_webos_shell_surface);
    qDebug() << __PRETTY_FUNCTION__;
    WebOSShellSurfacePrivate* shell = static_cast<WebOSShellSurfacePrivate*>(data);
    shell->m_position = QPointF(x, y);
    shell->positionChanged();
}


void WebOSShellSurfacePrivate::exposed(void *data, struct wl_webos_shell_surface *wl_webos_shell_surface, struct wl_array *rectangles)
{
    Q_UNUSED(wl_webos_shell_surface);
    WebOSShellSurfacePrivate* shell = static_cast<WebOSShellSurfacePrivate*>(data);
    int32_t* pos;

    QVector<QRect> rects;
    for (pos = (int32_t*)rectangles->data; pos < (int32_t*)rectangles->data + rectangles->size && *pos != -1; ) {
        if (pos + 4 < (int32_t*)rectangles->data + rectangles->size) {
            QRect r(*(pos + 0), *(pos + 1), *(pos + 2), *(pos + 3));
            rects << r;
        } else {
            qWarning() << "missing data from expose rects";
        }
        pos += 4;
    }

    QRegion exposeRegion;
    exposeRegion.setRects(rects.data(), rects.size());
    QWindowSystemInterface::handleExposeEvent(shell->m_parent->window(), exposeRegion);
}

void WebOSShellSurfacePrivate::positionChanged()
{
    Q_Q(WebOSShellSurface);
    q->emitPositionChanged();
}

void WebOSShellSurfacePrivate::state_about_to_change(void *data, struct wl_webos_shell_surface *wl_webos_shell_surface, uint32_t state)
{
    Q_UNUSED(wl_webos_shell_surface);
    WebOSShellSurfacePrivate* shell = static_cast<WebOSShellSurfacePrivate*>(data);
    Qt::WindowState qtState = qtWindowStateFromWaylandState(state);
    shell->stateAboutToChange(qtState);
}

void WebOSShellSurfacePrivate::stateAboutToChange(Qt::WindowState state)
{
    Q_Q(WebOSShellSurface);
    q->emitStateAboutToChange(state);
}

void WebOSShellSurfacePrivate::addon_status_changed(void *data, struct wl_webos_shell_surface *wl_webos_shell_surface, uint32_t addon_status)
{
    Q_UNUSED(wl_webos_shell_surface);
    WebOSShellSurfacePrivate* shell = static_cast<WebOSShellSurfacePrivate*>(data);
    WebOSShellSurface::AddonStatus status = addonStatusFromWaylandState(addon_status);
    shell->addonStatusChanged(status);
}

void WebOSShellSurfacePrivate::addonStatusChanged(WebOSShellSurface::AddonStatus status)
{
    Q_Q(WebOSShellSurface);
    q->emitAddonStatusChanged(status);
}

#if (QT_VERSION < QT_VERSION_CHECK(5,10,0))
void WebOSShellSurfacePrivate::setState(Qt::WindowState state)
#else
void WebOSShellSurfacePrivate::setState(Qt::WindowStates state)
#endif
{
    if (m_state == state)
        return;

    wl_webos_shell_surface_set_state(m_shellSurface, waylandStateFromQtWindowState(state));
}

void WebOSShellSurfacePrivate::setLocationHint(WebOSShellSurface::LocationHints hint)
{
    Q_Q(WebOSShellSurface);
    if (hint != m_locationHint) {
        m_locationHint = hint;
        wl_webos_shell_surface_set_location_hint(m_shellSurface, (uint32_t)m_locationHint);
        q->emitLocationHintChanged();
    }
}

WebOSShellSurface::LocationHints WebOSShellSurfacePrivate::locationHint()
{
    return m_locationHint;
}

void WebOSShellSurfacePrivate::setKeyMask(WebOSShellSurface::KeyMasks keyMask)
{
    Q_Q(WebOSShellSurface);
    if (keyMask != m_keyMask) {
        m_keyMask = keyMask;
        wl_webos_shell_surface_set_key_mask(m_shellSurface, (uint32_t)m_keyMask);
        q->emitKeyMaskChanged();
    }
}

WebOSShellSurface::KeyMasks WebOSShellSurfacePrivate::keyMask()
{
    return m_keyMask;
}

QVariantMap WebOSShellSurfacePrivate::properties() const
{
    return m_properties;
}

QVariant WebOSShellSurfacePrivate::property(const QString &name) const
{
    return m_properties.value(name);
}

QVariant WebOSShellSurfacePrivate::property(const QString &name, const QVariant &defaultValue) const
{
    return m_properties.value(name, defaultValue);
}

void WebOSShellSurfacePrivate::setProperty(const QString &name, const QVariant &value)
{
    if (!name.isEmpty()) {
        if (value.isValid() && value.canConvert(QMetaType::Bool) && value.toBool()) {
            if (name == "_WEBOS_ACCESS_POLICY_KEYS_HOME") {
                setKeyMask( WebOSShellSurface::KeyMaskHome | keyMask());
                qWarning() << "_WEBOS_ACCESS_POLICY_KEYS_HOME is deprecated, please use setKeyMask() instead";
            }
            if (name == "_WEBOS_ACCESS_POLICY_KEYS_BACK") {
                setKeyMask( WebOSShellSurface::KeyMaskBack | keyMask());
                qWarning() << "_WEBOS_ACCESS_POLICY_KEYS_BACK is deprecated, please use setKeyMask() instead";
            }
            if (name == "_WEBOS_ACCESS_POLICY_KEYS_EXIT") {
                setKeyMask( WebOSShellSurface::KeyMaskExit | keyMask());
                qWarning() << "_WEBOS_ACCESS_POLICY_KEYS_EXIT is deprecated, please use setKeyMask() instead";
            }
            if (name == "_WEBOS_ACCESS_POLICY_KEYS_GUIDE") {
                setKeyMask( WebOSShellSurface::KeyMaskGuide | keyMask());
                qWarning() << "_WEBOS_ACCESS_POLICY_KEYS_GUIDE is deprecated, please use setKeyMask() instead";
            }
        }
    }

    m_properties.insert(name, value);
    wl_webos_shell_surface_set_property(m_shellSurface, name.toLatin1().constData(), value.toString().toUtf8().constData());
}

void WebOSShellSurfacePrivate::sendProperty(const QString &name, const QVariant &value)
{
    setProperty(name, value);
}

void WebOSShellSurfacePrivate::setInputRegion(const QRegion& region)
{
    QPlatformNativeInterface *wliface = QGuiApplication::platformNativeInterface();
    wl_compositor *wlcompositor = static_cast<wl_compositor *>(wliface->nativeResourceForIntegration("compositor"));
    wl_region *wlregion = wl_compositor_create_region(wlcompositor);
    qreal dpr = m_parent->window()->devicePixelRatio();

    for (auto &rect: region) {
        wl_region_add(wlregion, rect.x()*dpr, rect.y()*dpr,
                      rect.width()*dpr, rect.height()*dpr);
    }

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    wl_surface *wlsurface = m_parent->wlSurface();
#else
    wl_surface *wlsurface = static_cast<QtWayland::wl_surface *>(m_parent)->object();
#endif
    wl_surface_set_input_region(wlsurface, wlregion);
    wl_surface_commit(wlsurface);
    wl_region_destroy(wlregion);
}

void WebOSShellSurfacePrivate::setAddon(const QString& addon)
{
    Q_Q(WebOSShellSurface);
    if (addon != m_addon) {
        m_addon = addon;
        wl_webos_shell_surface_set_addon(m_shellSurface, m_addon.toUtf8().constData());
        q->emitAddonChanged();
    }
}

void WebOSShellSurfacePrivate::resetAddon()
{
    Q_Q(WebOSShellSurface);
    if (!m_addon.isEmpty()) {
        wl_webos_shell_surface_reset_addon(m_shellSurface);
        m_addon.clear();
        q->emitAddonChanged();
    }
}

QString WebOSShellSurfacePrivate::addon() const
{
    return m_addon;
}

WebOSShellSurface::WebOSShellSurface(wl_webos_shell_surface* shellSurface, struct ::wl_shell_surface *shell_surface, QPlatformWindow* parent)
    : d_ptr(new WebOSShellSurfacePrivate(shellSurface, shell_surface, parent))
{
    Q_D(WebOSShellSurface);
    d->q_ptr = this;
}

WebOSShellSurface::~WebOSShellSurface()
{
}

QPointF WebOSShellSurface::position()
{
    Q_D(WebOSShellSurface);
    return d->position();
}

void WebOSShellSurface::emitPositionChanged()
{
    emit positionChanged();
}

void WebOSShellSurface::emitStateAboutToChange(Qt::WindowState state)
{
    emit stateAboutToChange(state);
}


#if (QT_VERSION < QT_VERSION_CHECK(5,10,0))
void WebOSShellSurface::setState(Qt::WindowState state)
#else
void WebOSShellSurface::setState(Qt::WindowStates state)
#endif
{
    Q_D(WebOSShellSurface);
    d->setState(state);
}

QVariantMap WebOSShellSurface::properties() const
{
    const Q_D(WebOSShellSurface);
    return d->properties();
}

QVariant WebOSShellSurface::property(const QString &name) const
{
    const Q_D(WebOSShellSurface);
    return d->property(name);
}

QVariant WebOSShellSurface::property(const QString &name, const QVariant &defaultValue) const
{
    const Q_D(WebOSShellSurface);
    return d->property(name, defaultValue);
}

void WebOSShellSurface::setProperty(const QString &name, const QVariant &value)
{
    Q_D(WebOSShellSurface);
    d->setProperty(name, value);
}

WebOSShellSurface::LocationHints WebOSShellSurface::locationHint()
{
    Q_D(WebOSShellSurface);
    return d->locationHint();
}

void WebOSShellSurface::setLocationHint(WebOSShellSurface::LocationHints hint)
{
    Q_D(WebOSShellSurface);
    d->setLocationHint(hint);
}

void WebOSShellSurface::emitLocationHintChanged()
{
    emit locationHintChanged();
}

WebOSShellSurface::KeyMasks WebOSShellSurface::keyMask()
{
    Q_D(WebOSShellSurface);
    return d->keyMask();
}

void WebOSShellSurface::setKeyMask(WebOSShellSurface::KeyMasks keyMask)
{
    Q_D(WebOSShellSurface);
    d->setKeyMask(keyMask);
}

void WebOSShellSurface::emitKeyMaskChanged()
{
    emit keyMaskChanged();
}

void WebOSShellSurface::setInputRegion(const QRegion& region)
{
    Q_D(WebOSShellSurface);
    d->setInputRegion(region);
}

QString WebOSShellSurface::addon()
{
    Q_D(WebOSShellSurface);
    return d->addon();
}

void WebOSShellSurface::setAddon(const QString& addon)
{
    Q_D(WebOSShellSurface);
    d->setAddon(addon);
}

void WebOSShellSurface::resetAddon()
{
    Q_D(WebOSShellSurface);
    d->resetAddon();
}

void WebOSShellSurface::emitAddonChanged()
{
    emit addonChanged();
}

void WebOSShellSurface::emitAddonStatusChanged(AddonStatus status)
{
    emit addonStatusChanged(status);
}
