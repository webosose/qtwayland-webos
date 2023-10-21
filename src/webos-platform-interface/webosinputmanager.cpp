// Copyright (c) 2014-2018 LG Electronics, Inc.
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

#include "webosinputmanager.h"
#include "webosinputmanager_p.h"
#include "securecoding.h"

#include <QtWaylandClient/private/qwaylanddisplay_p.h>
#include <QtWaylandClient/private/qwaylandwindow_p.h>

WebOSInputManagerPrivate::WebOSInputManagerPrivate(QWaylandDisplay* display, uint32_t id)
    : QtWayland::wl_webos_input_manager(display->wl_registry(), uint2int(id), 1)
    , q_ptr(0)
{}

void WebOSInputManagerPrivate::webos_input_manager_cursor_visibility(uint32_t visibility,
                                                                struct ::wl_webos_seat *webos_seat)
{
    Q_UNUSED(webos_seat); //Qt haven't supported multi-input yet, so just ignore it.
    Q_Q(WebOSInputManager);
    q->setCursorVisible(visibility?true:false);
}

WebOSInputManagerPrivate::~WebOSInputManagerPrivate()
{
    q_ptr = NULL;
}

WebOSInputManager::WebOSInputManager(QWaylandDisplay* display, uint32_t id)
    : d_ptr(new WebOSInputManagerPrivate(display, id))
    , m_mouseVisible(false)
{
    Q_D(WebOSInputManager);
    d->q_ptr = this;
}

void WebOSInputManager::setCursorVisible(bool visible)
{
    if (m_mouseVisible != visible) {
        m_mouseVisible = visible;
        emit cursorVisibleChanged(visible);
    }
}

void WebOSInputManager::requestChangeCursorVisibility(bool visibility)
{
    Q_D(WebOSInputManager);
    d->set_cursor_visibility(visibility);
}

WebOSInputManager::~WebOSInputManager()
{
}
