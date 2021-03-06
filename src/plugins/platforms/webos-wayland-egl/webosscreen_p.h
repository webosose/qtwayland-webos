// Copyright (c) 2015-2021 LG Electronics, Inc.
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

#ifndef WEBOSSCREEN_H
#define WEBOSSCREEN_H

#include <QtWaylandClient/private/qwaylandscreen_p.h>

using QtWaylandClient::QWaylandScreen;
using QtWaylandClient::QWaylandDisplay;

class WebOSScreen : public QObject, public QWaylandScreen
{
    Q_OBJECT
public:
    WebOSScreen(QWaylandDisplay *display, int version, uint32_t id);

    qreal devicePixelRatio() const override;

    void output_done() override;

    int currentTransform() const { return mCurrentTransform; }
    static bool compareOutputTransform(const int& a, const int& b) { return (a % 2 != b % 2); }

signals:
    void outputTransformChanged();
    void devicePixelRatioChanged();

private:
    void updateDevicePixelRatio();

    qreal mDevicePixelRatio;
    int mCurrentTransform;
};

#endif
