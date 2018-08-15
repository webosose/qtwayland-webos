// Copyright (c) 2015-2019 LG Electronics, Inc.
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

#ifndef WEBOSNATIVEINTERFACE_H
#define WEBOSNATIVEINTERFACE_H

#include <QtWaylandClient/private/qwaylandnativeinterface_p.h>

using QtWaylandClient::QWaylandNativeInterface;
using QtWaylandClient::QWaylandIntegration;

class WebOSNativeInterface : public QWaylandNativeInterface
{
public:
    WebOSNativeInterface(QWaylandIntegration *integration);

    void *nativeResourceForScreen(const QByteArray &resourceString, QScreen *screen);
#ifdef HAS_CRIU
    void *nativeResourceForIntegration(const QByteArray &resourceString);
#endif
};

#endif
