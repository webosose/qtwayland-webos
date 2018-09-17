// Copyright (c) 2018 LG Electronics, Inc.
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

#ifndef WEBOSINPUTPANELLOCATOR_H
#define WEBOSINPUTPANELLOCATOR_H

#include <stdint.h>

#include <QObject>
#include <QRect>

class WebOSInputPanelLocatorPrivate;

class WebOSInputPanelLocator : public QObject {

    Q_OBJECT

public:
    static WebOSInputPanelLocator* instance();
    ~WebOSInputPanelLocator() override;

    void setInputPanelRect(const QObject* focusObj, int32_t x, int32_t y, uint32_t width, uint32_t height);
    QRect inputPanelRect(const QObject* focusObj) const;

Q_SIGNALS:
    void inputPanelRectChanged(const QObject* focusObj, const QRect& rect);

private:
    WebOSInputPanelLocator();
    static WebOSInputPanelLocator* s_instance;

    QScopedPointer<WebOSInputPanelLocatorPrivate> d_ptr;

    Q_DECLARE_PRIVATE(WebOSInputPanelLocator);
    Q_DISABLE_COPY(WebOSInputPanelLocator);
};
#endif
