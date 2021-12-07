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

#include "webospresentationtime_p.h"
#include "webospresentationtime.h"

#include <QtWaylandClient/private/qwaylanddisplay_p.h>
#include <QtWaylandClient/private/qwaylandwindow_p.h>
#include <QtWaylandClient/private/qwaylandscreen_p.h>

WebOSPresentationFeedbackPrivate::WebOSPresentationFeedbackPrivate(struct ::wp_presentation_feedback *object)
: wp_presentation_feedback(object)
{
}

void WebOSPresentationFeedbackPrivate::wp_presentation_feedback_sync_output(struct ::wl_output *output)
{
    emit syncOutput(QtWaylandClient::QWaylandScreen::fromWlOutput(output));
}

void WebOSPresentationFeedbackPrivate::wp_presentation_feedback_presented(uint32_t tv_sec_hi, uint32_t tv_sec_lo, uint32_t tv_nsec, uint32_t refresh, uint32_t seq_hi, uint32_t seq_lo, uint32_t flags)
{
    emit presented(tv_sec_hi, tv_sec_lo, tv_nsec, refresh, seq_hi, seq_lo, flags);
}

void WebOSPresentationFeedbackPrivate::wp_presentation_feedback_discarded()
{
    emit discarded();
}

WebOSPresentationTimePrivate::WebOSPresentationTimePrivate(struct ::wl_registry *registry, uint32_t id, int version)
    : QtWayland::wp_presentation(registry, id, version)
{

}

void WebOSPresentationTimePrivate::wp_presentation_clock_id(uint32_t clk_id)
{
    qInfo() << "WebOSPresentationTime set clock id to" << clk_id;
    m_clock_id = clk_id;
}

WebOSPresentationTime::WebOSPresentationTime(QWaylandDisplay *display, uint32_t id)
    : QObject(*new WebOSPresentationTimePrivate(display->wl_registry(), id, 1))
{
}

void WebOSPresentationTime::requestFeedback(QWaylandWindow *window)
{
    Q_D(WebOSPresentationTime);

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    auto *surface = window->wlSurface();
#else
    auto *surface = window->object();
#endif
    auto *feedback = new WebOSPresentationFeedbackPrivate(d->feedback(surface));

    if (feedback) {
        connect(feedback, &WebOSPresentationFeedbackPrivate::syncOutput, this, &WebOSPresentationTime::feedbackSyncOutput);
        connect(feedback, &WebOSPresentationFeedbackPrivate::presented, this, &WebOSPresentationTime::feedbackPresented);
        connect(feedback, &WebOSPresentationFeedbackPrivate::discarded, this, &WebOSPresentationTime::feedbackDiscarded);

        struct timespec ts;
        clock_gettime(d->clock_id(), &ts);

        mFeedbacks[feedback] = ts;
    }
}

static int
timespec_diff_to_usec(const struct timespec *a, const struct timespec *b)
{
    time_t secs = a->tv_sec - b->tv_sec;
    long nsec = a->tv_nsec - b->tv_nsec;

    return secs * 1000000 + nsec / 1000;
}

static inline void
timespec_from_proto(struct timespec *a, uint32_t tv_sec_hi,
        uint32_t tv_sec_lo, uint32_t tv_nsec)
{
    a->tv_sec = ((uint64_t)tv_sec_hi << 32) + tv_sec_lo;
    a->tv_nsec = tv_nsec;
}


void WebOSPresentationTime::feedbackSyncOutput(QWaylandScreen *screen)
{
    //TODO: Distinguish screens for the presentation
}

void WebOSPresentationTime::feedbackPresented(uint32_t tv_sec_hi, uint32_t tv_sec_lo, uint32_t tv_nsec, uint32_t refresh, uint32_t seq_hi, uint32_t seq_lo, uint32_t flags)
{
    static struct timespec prevPt= {0, 0};
    auto *feedback = qobject_cast<WebOSPresentationFeedbackPrivate *>(sender());
    if (!feedback) {
        qWarning("Invalid feedback");
        return;
    }

    if (mFeedbacks.contains(feedback)) {
        struct timespec pt;
        timespec_from_proto(&pt, tv_sec_hi, tv_sec_lo, tv_nsec);

        // deliverUpdateRequestToPresentation
        uint32_t d2p = timespec_diff_to_usec(&pt, &mFeedbacks[feedback]);
        // between Presentations
        uint32_t p2p = timespec_diff_to_usec(&pt, &prevPt);

        emit presented(d2p, p2p);

        mFeedbacks.remove(feedback);
        prevPt = pt;
    }
    disconnect(feedback);
    feedback->deleteLater();
}

void WebOSPresentationTime::feedbackDiscarded()
{
    auto *feedback = qobject_cast<WebOSPresentationFeedbackPrivate *>(sender());
    if (!feedback) {
        qWarning("Invalid feedback");
        return;
    }
    qWarning() << "feedback discarded";

    mFeedbacks.remove(feedback);
    disconnect(feedback);
    feedback->deleteLater();
}
