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

#ifndef WEBOSSINPUTMANAGER_H
#define WEBOSSINPUTMANAGER_H

#include <stdint.h>

#include <QObject>

class QWaylandDisplay;
class WebOSInputManagerPrivate;

class WebOSInputManager : public QObject {

    Q_OBJECT
public:
    ~WebOSInputManager();
    void setCursorVisible(bool visible);
    bool getCursorVisible() const {return m_mouseVisible;}

signals:
    void cursorVisibleChanged(bool visible);

private:
    WebOSInputManager(QWaylandDisplay* disp, uint32_t id);
    bool m_mouseVisible;

    QScopedPointer<WebOSInputManagerPrivate> d_ptr;
    Q_DECLARE_PRIVATE(WebOSInputManager);
    Q_DISABLE_COPY(WebOSInputManager);

    friend class WebOSPlatformPrivate;
};
#endif
