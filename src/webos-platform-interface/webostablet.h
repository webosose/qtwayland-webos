/* @@@LICENSE
 *
 *      Copyright (c) 2018-2020 LG Electronics, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * LICENSE@@@ */

#ifndef WEBOSTABLET_H
#define WEBOSTABLET_H

#include <stdint.h>
#include <wayland-server.h>

#include <QObject>

namespace QtWaylandClient {
    class QWaylandDisplay;
}

using QtWaylandClient::QWaylandDisplay;
class WebOSTabletPrivate;

class WebOSTablet : public QObject {

    Q_OBJECT
public:
    ~WebOSTablet();

private:
    WebOSTablet(QWaylandDisplay* disp, uint32_t id);

    QScopedPointer<WebOSTabletPrivate> d_ptr;
    Q_DECLARE_PRIVATE(WebOSTablet);
    Q_DISABLE_COPY(WebOSTablet);

    friend class WebOSPlatformPrivate;
};
#endif
