// Copyright (c) 2013-2018 LG Electronics, Inc.
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

#include "webosxinput.h"

#include <qpa/qplatformnativeinterface.h>
#include <QtGui/QGuiApplication>
#include <QDebug>

const struct wl_registry_listener WebOSXInputExtension::registryListener = {
    WebOSXInputExtension::registryGlobalAdded,
    WebOSXInputExtension::registryGlobalRemoved
};


WebOSXInputExtension::WebOSXInputExtension()
    : m_extension(0)
    , m_display(0)
    , m_registry(0)
{
    QPlatformNativeInterface *nativeInterface = QGuiApplication::platformNativeInterface();
    if (nativeInterface) {
        m_display = static_cast<wl_display*>(nativeInterface->nativeResourceForIntegration("display"));
    }
    Q_ASSERT(m_display);

    m_registry = wl_display_get_registry(m_display);
    wl_registry_add_listener(m_registry, &registryListener, this);
}

WebOSXInputExtension::~WebOSXInputExtension()
{
}

void WebOSXInputExtension::registryGlobalAdded(void *data, struct wl_registry *registry, uint32_t id, const char *interface, uint32_t version)
{
    Q_UNUSED(data);
    Q_UNUSED(registry);
    Q_UNUSED(version);

    QByteArray interfaceName(interface);
    WebOSXInputExtension* that = static_cast<WebOSXInputExtension*>(data);
    if (interfaceName == "wl_webos_xinput_extension") {
        that->m_extension = static_cast<wl_webos_xinput_extension*>(wl_registry_bind(that->m_registry, id, &wl_webos_xinput_extension_interface, 1));
        emit that->interfaceReady();
    }
}

void WebOSXInputExtension::registryGlobalRemoved(void *data, wl_registry *registry, uint32_t name)
{
    Q_UNUSED(data);
    Q_UNUSED(registry);
    Q_UNUSED(name);
}

wl_webos_xinput* WebOSXInputExtension::registerInput()
{
    Q_ASSERT(m_extension);
    return wl_webos_xinput_extension_register_input(m_extension);
}


const struct wl_webos_xinput_listener WebOSXInput::m_inputListener = {
    WebOSXInput::activationRequest,
    WebOSXInput::deactivationRequest
};


WebOSXInput::WebOSXInput()
    : m_extension(new WebOSXInputExtension)
    , m_input(0)
{
    connect(m_extension, SIGNAL(interfaceReady()), this, SLOT(doRegister()));
}

WebOSXInput::~WebOSXInput()
{
}

bool WebOSXInput::activate(const QString& type)
{
    if (!m_input) {
        qWarning() << "webos_xinput is not bound, type:" << type;
        return false;
    }

    wl_webos_xinput_activated(m_input, type.toLatin1().constData());
    return true;
}

bool WebOSXInput::deactivate()
{
    if (!m_input) {
        qWarning() << "webos_xinput is not bound";
        return false;
    }

    wl_webos_xinput_deactivated(m_input);
    return true;
}

bool WebOSXInput::invokeAction(uint32_t keysym, SymbolType symType, EventType eventType)
{
    if (!m_input) {
        qWarning() << "webos_xinput is not bound, params:" << keysym << symType << eventType;
        return false;
    }

    wl_webos_xinput_invoke_action(m_input, keysym, (int)symType, (int)eventType);
    return true;
}

void WebOSXInput::activationRequest(void *data, struct wl_webos_xinput *wl_webos_xinput, const char *type)
{
    Q_UNUSED(wl_webos_xinput);
    WebOSXInput* that = static_cast<WebOSXInput*>(data);
    emit that->activationRequested(QString(type));
}

void WebOSXInput::deactivationRequest(void *data, struct wl_webos_xinput *wl_webos_xinput)
{
    Q_UNUSED(wl_webos_xinput);
    WebOSXInput* that = static_cast<WebOSXInput*>(data);
    emit that->deactivationRequested();
}

void WebOSXInput::doRegister()
{
    m_input = m_extension->registerInput();
    wl_webos_xinput_add_listener(m_input, &m_inputListener, this);

    emit registered();
}
