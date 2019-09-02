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

#include <QDebug>
#include "webosinputpanellocator.h"
#include "webosinputpanellocator_p.h"

WebOSInputPanelLocatorPrivate::WebOSInputPanelLocatorPrivate(
    WebOSInputPanelLocator* q)
    : QObject(q)
    , q_ptr(q)
{
}

void WebOSInputPanelLocatorPrivate::setInputPanelRect(const QObject* focusObj,
                                            int32_t x, int32_t y,
                                            uint32_t width, uint32_t height)
{
    auto it = m_inputPanelRect.find(focusObj);
    if (it != m_inputPanelRect.end()) {
        it.value().setRect(x, y, width, height);
    } else {
        connect(focusObj, &QObject::destroyed,
                this, &WebOSInputPanelLocatorPrivate::focusObjectDestroyed);
        m_inputPanelRect.insert(focusObj, QRect(x, y, width, height));
    }

    Q_Q(WebOSInputPanelLocator);
    emit q->inputPanelRectChanged(focusObj, m_inputPanelRect[focusObj]);
}

QRect WebOSInputPanelLocatorPrivate::inputPanelRect(const QObject* focusObj) const
{
    auto it = m_inputPanelRect.find(focusObj);
    if (it != m_inputPanelRect.constEnd())
        return it.value();
    else
        return QRect(0, 0, 0, 0);
}

void WebOSInputPanelLocatorPrivate::focusObjectDestroyed(QObject* focusObj)
{
    m_inputPanelRect.remove(focusObj);
}

WebOSInputPanelLocator::WebOSInputPanelLocator()
    : d_ptr(new WebOSInputPanelLocatorPrivate(this))
{
}

WebOSInputPanelLocator* WebOSInputPanelLocator::s_instance = nullptr;

WebOSInputPanelLocator* WebOSInputPanelLocator::instance()
{
    if (s_instance == nullptr)
        s_instance = new WebOSInputPanelLocator();
    return s_instance;
}

void WebOSInputPanelLocator::setInputPanelRect(const QObject* focusObj, int32_t x, int32_t y, uint32_t width, uint32_t height)
{
    d_ptr->setInputPanelRect(focusObj, x, y, width, height);
}

QRect WebOSInputPanelLocator::inputPanelRect(const QObject* focusObj) const
{
    return d_ptr->inputPanelRect(focusObj);
}

WebOSInputPanelLocator::~WebOSInputPanelLocator()
{
}
