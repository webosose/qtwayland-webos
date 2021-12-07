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

#ifndef WEBOSPRESENTATIONTIME_P_H
#define WEBOSPRESENTATIONTIME_P_H

#include "qwayland-presentation-time.h"
#include "wayland-presentation-time-client-protocol.h"
#include "webospresentationtime.h"

#include <QtWaylandClient/private/qtwaylandclientglobal_p.h>
#include <QtWaylandClient/private/qwayland-wayland.h>
#include <QtCore/private/qobject_p.h>

namespace QtWaylandClient {
    class QWaylandScreen;
}

class WebOSPresentationTimePrivate : public QObjectPrivate, public QtWayland::wp_presentation
{
    Q_DECLARE_PUBLIC(WebOSPresentationTime)
public:
    WebOSPresentationTimePrivate(struct ::wl_registry *registry, uint32_t id, int version);

    uint32_t clock_id() const { return m_clock_id; }

private:
    void wp_presentation_clock_id(uint32_t) override;

    uint32_t m_clock_id = CLOCK_MONOTONIC;
};

class WebOSPresentationFeedbackPrivate : public QObject, public QtWayland::wp_presentation_feedback
{
    Q_OBJECT
public:
    explicit WebOSPresentationFeedbackPrivate(struct ::wp_presentation_feedback *object);

signals:
    void syncOutput(QtWaylandClient::QWaylandScreen *screen);
    void presented(uint32_t tv_sec_hi, uint32_t tv_sec_lo, uint32_t tv_nsec, uint32_t refresh, uint32_t seq_hi, uint32_t seq_lo, uint32_t flags);
    void discarded();

protected:
    void wp_presentation_feedback_sync_output(struct ::wl_output *output) override;
    void wp_presentation_feedback_presented(uint32_t tv_sec_hi, uint32_t tv_sec_lo, uint32_t tv_nsec, uint32_t refresh, uint32_t seq_hi, uint32_t seq_lo, uint32_t flags) override;
    void wp_presentation_feedback_discarded() override;
};

#endif // WEBOSPRESENTATIONTIME_P_H
