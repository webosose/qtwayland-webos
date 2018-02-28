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

#ifndef WEBOSXINPUT_H
#define WEBOSXINPUT_H

#include <QObject>

#include <wayland-client.h>
#include <wayland-webos-extension-client-protocol.h>

class WebOSXInputExtension;

class WebOSXInput : public QObject {

    Q_OBJECT

public:
    WebOSXInput();
    virtual ~WebOSXInput();

    /*!
     * The client is reposible for calling this method when the input becomes active
     * either via internally a condition rises or when the activationRequest is called
     * with the correct type.
     */
    bool activate(const QString& type = "");

    /*!
     * Must be called when the input is finished and no longer needed or when the system
     * requests deactivation.
     */
    bool deactivate();

    enum SymbolType {
        Qt = 1,
        Native
    };

    enum EventType {
        PressAndRelease = 0,
        Press,
        Release
    };

    /*!
     * Called to invoke an action on the system. The type parameter describes the domain
     * of the key code.
     */
    bool invokeAction(uint32_t keysym, SymbolType symType = Qt, EventType eventType = PressAndRelease);

signals:
    /*!
     * Emitted when the client was registered to the server. It means a proxy has been created
     * and it is ready to communicate with the server.
     */
    void registered();

    /*!
     * Emitted when the system wants to activate an input method. The client is responsible
     * for filtering out the correct input method for itself and not activate blindly. The
     * client must respond with 'activate()' if the correct type is received.
     */
    void activationRequested(const QString& type);

    /*!
     * Emitted when the system wants to deactivate an input method. The
     * client must respond with 'deactivated()'.
     */
    void deactivationRequested();

private:
    WebOSXInputExtension* m_extension;
    wl_webos_xinput* m_input;

    static void activationRequest(void *data, struct wl_webos_xinput *wl_webos_xinput, const char *type);
    static void deactivationRequest(void *data, struct wl_webos_xinput *wl_webos_xinput);
    static const struct wl_webos_xinput_listener m_inputListener;

private slots:
    void doRegister();
};

class WebOSXInputExtension : public QObject {

    Q_OBJECT

public:
    WebOSXInputExtension();
    ~WebOSXInputExtension();

    wl_webos_xinput* registerInput();

signals:
    void interfaceReady();

private:
    wl_webos_xinput_extension* m_extension;
    wl_display* m_display;
    wl_registry* m_registry;

    static const struct wl_registry_listener registryListener;
    static void registryGlobalAdded(void *data, struct wl_registry *registry, uint32_t id, const char *interface, uint32_t version);
    static void registryGlobalRemoved(void *data, wl_registry *registry, uint32_t name);

};
#endif // WEBOSXINPUT_H
