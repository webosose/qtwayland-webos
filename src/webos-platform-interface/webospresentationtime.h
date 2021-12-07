// Copyright (c) 2021 LG Electronics, Inc.
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

#ifndef WEBOSPRESENTATIONTIME_H
#define WEBOSPRESENTATIONTIME_H

#include <QObject>
#include <QMap>
#include <QScopedPointer>
#include <time.h>

namespace QtWaylandClient {
    class QWaylandScreen;
    class QWaylandWindow;
    class QWaylandDisplay;
}
using QtWaylandClient::QWaylandDisplay;
using QtWaylandClient::QWaylandWindow;
using QtWaylandClient::QWaylandScreen;


class WebOSPresentationTimePrivate;
class WebOSPresentationFeedbackPrivate;

class WebOSPresentationTime: public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(WebOSPresentationTime);
public:
    WebOSPresentationTime(QWaylandDisplay *display, uint32_t id);

    void requestFeedback(QWaylandWindow *window);

signals:
    // DeliverUpdateToPresented, PrevPresentedToThis
    void presented(uint32_t d2p, uint32_t p2p);

private slots:
    void feedbackSyncOutput(QWaylandScreen *screen);
    void feedbackPresented(uint32_t tv_sec_hi, uint32_t tv_sec_lo, uint32_t tv_nsec, uint32_t refresh, uint32_t seq_hi, uint32_t seq_lo, uint32_t flags);
    void feedbackDiscarded();

private:
    QMap<WebOSPresentationFeedbackPrivate *, struct timespec> mFeedbacks;
};

#endif // WEBOSPRESENTATIONTIME_H
