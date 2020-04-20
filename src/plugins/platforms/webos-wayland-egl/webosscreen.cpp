// Copyright (c) 2015-2020 LG Electronics, Inc.
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

#include <QByteArray>
#include <QString>
#include <QRegExp>
#include <QUrlQuery>
#include <QDebug>

#include <qpa/qwindowsysteminterface.h>

#include "webosscreen_p.h"

WebOSScreen::WebOSScreen(QWaylandDisplay *display, int version, uint32_t id)
    : QWaylandScreen(display, version, id)
    , mDevicePixelRatio(1.0)
    , mCurrentTransform(-1)
{
}

qreal WebOSScreen::devicePixelRatio() const
{
    return mDevicePixelRatio;
}

void WebOSScreen::updateDevicePixelRatio()
{
    // Update devicePixelRatio based on WEBOS_DEVICE_PIXEL_RATIO is set
    // Valid values are:
    // 1) WEBOS_DEVICE_PIXEL_RATIO=auto (requires WEBOS_WINDOW_BASE_GEOMETRY)
    // 2) WEBOS_DEVICE_PIXEL_RATIO=<ratio>

    QByteArray dpr = qgetenv("WEBOS_DEVICE_PIXEL_RATIO");
    QString geom(qgetenv("WEBOS_WINDOW_BASE_GEOMETRY"));
    QRegExp pattern("([0-9]+)x([0-9]+)");
    qreal ratio = (qreal) dpr.toDouble();

    if (ratio > 0.0) {
        qInfo() << "Set devicePixelRatio as" << ratio << "by WEBOS_DEVICE_PIXEL_RATIO";
    } else if (dpr.startsWith("auto") && pattern.indexIn(geom) >= 0) {
        QStringList list = pattern.capturedTexts();
        int screenWidth = 0;
        int screenHeight = 0;
        switch (mCurrentTransform) {
        case WL_OUTPUT_TRANSFORM_90:
        case WL_OUTPUT_TRANSFORM_270:
        case WL_OUTPUT_TRANSFORM_FLIPPED_90:
        case WL_OUTPUT_TRANSFORM_FLIPPED_270:
            screenWidth = geometry().height();
            screenHeight = geometry().width();
            break;
        default:
            screenWidth = geometry().width();
            screenHeight = geometry().height();
            break;
        }
        // Window base geometry: indicates the maximum window size in fullscreen
        int w = list.at(1).toInt();
        int h = list.at(2).toInt();
        if (w <= 0 && h <= 0)
            ratio = 1.0;
        else if (w <= 0)
            ratio = (qreal) screenHeight / h;
        else if (h <= 0)
            ratio = (qreal) screenWidth / w;
        else
            ratio = qMin((qreal) screenWidth / w, (qreal) screenHeight / h);
        qInfo() << "Set auto devicePixelRatio as" << ratio << "screen:" << screenWidth << screenHeight << "window base geometry:" << w << h;
    }

    if (ratio <= 0.0) {
        ratio = 1.0;
        qInfo() << "Use default devicePixelRatio" << ratio;
    }

    if (mDevicePixelRatio != ratio) {
        mDevicePixelRatio = ratio;
        qInfo() << "Set devicePixelRatio to" << mDevicePixelRatio;
        emit devicePixelRatioChanged();
    }
}

void WebOSScreen::output_done()
{
    // Get output name from the model string
    mOutputName = QUrlQuery(mModel).queryItemValue(QStringLiteral("name"));

    if (mTransform >= 0 && mCurrentTransform != mTransform) {
        int oldTransform = mCurrentTransform;
        mCurrentTransform = mTransform;
        emit outputTransformChanged(oldTransform, mCurrentTransform);
    }
    QWaylandScreen::output_done();
    updateDevicePixelRatio();
}
