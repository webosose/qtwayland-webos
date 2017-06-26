// Copyright (c) 2017-2018 LG Electronics, Inc.
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

#include "webosappsnapshotmanager.h"
#include "appsnapshotmanager_p.h"
#include "qwaylandeglclientbufferintegration.h"
#include "qwaylandeglinclude.h"

#include <QDebug>
#include <QSocketNotifier>
#include <QThread>
#include <QtCore/qcoreapplication.h>
#include <QtGui/private/qguiapplication_p.h>
#include <QtWaylandClient/private/qwaylanddisplay_p.h>
#include <QtWaylandClient/private/qwaylandintegration_p.h>
#include <QtWaylandClient/private/qwaylandscreen_p.h>

class WebOSAppSnapshotManagerPrivate : public AppSnapshotManagerPrivate
{
public:
    Q_DECLARE_PUBLIC(WebOSAppSnapshotManager)

    WebOSAppSnapshotManagerPrivate(QWaylandDisplay* dpy, QWaylandEglClientBufferIntegration* cbi);

    bool preDumpInternal() Q_DECL_OVERRIDE;
    bool postDumpInternal() Q_DECL_OVERRIDE;
    bool restoreInternal() Q_DECL_OVERRIDE;

    void recover();

    void initializeWaylandIntegration();
    void connectToEventDispatcher(QAbstractEventDispatcher* dispatcher);
    void disconnectToEventDispatcher(QAbstractEventDispatcher* dispatcher);

    QWaylandDisplay* m_display;
    QWaylandEglClientBufferIntegration* m_clientBufferIntegration;
    bool m_displayDestroyed;
    bool m_clientBufferIntegrationDestroyed;
};


template <typename T> static void destroyAndLeaveDangling(const T* ptr)
{
    T* tmp = static_cast<T*>(malloc(sizeof(T)));
    memcpy(tmp, ptr, sizeof(T));

    delete tmp;
}

WebOSAppSnapshotManagerPrivate::WebOSAppSnapshotManagerPrivate(QWaylandDisplay* dpy, QWaylandEglClientBufferIntegration* cbi)
    : AppSnapshotManagerPrivate()
    , m_display(dpy)
    , m_clientBufferIntegration(cbi)
    , m_displayDestroyed(false)
    , m_clientBufferIntegrationDestroyed(false)
{
}

bool WebOSAppSnapshotManagerPrivate::preDumpInternal()
{
    Q_ASSERT(m_display);
    Q_ASSERT(m_clientBufferIntegration);

    if (!m_clientBufferIntegrationDestroyed) {
        destroyAndLeaveDangling(m_clientBufferIntegration);
        m_clientBufferIntegrationDestroyed = true;

        typedef EGLBoolean (EGLAPIENTRYP EGLDESTRUCTORLOADERLG) (void);
        EGLDESTRUCTORLOADERLG pEglDestructorLoaderLG = reinterpret_cast<EGLDESTRUCTORLOADERLG>(eglGetProcAddress("eglDestructorLoaderLG"));
        if (pEglDestructorLoaderLG)
            pEglDestructorLoaderLG();
    }

    if (!m_displayDestroyed) {
        QList<QWaylandScreen*> screens = m_display->screens();
        QWaylandIntegration* win = static_cast<QWaylandIntegration*>(QGuiApplicationPrivate::platformIntegration());
        foreach (QWaylandScreen *screen, screens) {
            win->removeScreen(screen->screen());
            delete screen->screen();
        }

        destroyAndLeaveDangling(m_display);
        m_displayDestroyed = true;
    }

    return AppSnapshotManagerPrivate::preDumpInternal();
}

bool WebOSAppSnapshotManagerPrivate::postDumpInternal()
{
    // CRIU library callbacks a post-dump function after creating a snapshot image.
    // We expect same processing at post-dump and restore callback, which is to recover the destroyed instance.
    recover();
    return AppSnapshotManagerPrivate::postDumpInternal();
}

bool WebOSAppSnapshotManagerPrivate::restoreInternal()
{
    // CRIU library callbacks a restore function after forking a process from the snapshot image.
    // We expect same processing at post-dump and restore callback, which is to recover the destroyed instance.
    recover();
    return AppSnapshotManagerPrivate::restoreInternal();
}

void WebOSAppSnapshotManagerPrivate::recover()
{
    Q_ASSERT(m_display);
    Q_ASSERT(m_clientBufferIntegration);

    if (m_displayDestroyed) {
        connectToEventDispatcher(QGuiApplicationPrivate::eventDispatcher);
        m_display = new (m_display) QWaylandDisplay(static_cast<QWaylandIntegration*>(QGuiApplicationPrivate::platformIntegration()));
        m_displayDestroyed = false;
    }

    if (m_clientBufferIntegrationDestroyed) {
        m_clientBufferIntegration = new (m_clientBufferIntegration) QWaylandEglClientBufferIntegration;
        m_clientBufferIntegrationDestroyed = false;
        m_clientBufferIntegration->initialize(m_display);
    }
}

void WebOSAppSnapshotManagerPrivate::initializeWaylandIntegration()
{
    QWaylandIntegration *wayland_integration = static_cast<QWaylandIntegration *>(QGuiApplicationPrivate::platformIntegration());
    wayland_integration->initialize();
}

void WebOSAppSnapshotManagerPrivate::connectToEventDispatcher(QAbstractEventDispatcher* dispatcher)
{
    Q_Q(WebOSAppSnapshotManager);
    QObject::connect(dispatcher, SIGNAL(aboutToBlock()), q, SLOT(onAboutToBlock()));
}

void WebOSAppSnapshotManagerPrivate::disconnectToEventDispatcher(QAbstractEventDispatcher* dispatcher)
{
    Q_Q(WebOSAppSnapshotManager);
    QObject::disconnect(dispatcher, SIGNAL(aboutToBlock()), q, SLOT(onAboutToBlock()));
}

WebOSAppSnapshotManager::WebOSAppSnapshotManager(QWaylandDisplay* dpy, QWaylandEglClientBufferIntegration* cbi)
    : AppSnapshotManager(*(new WebOSAppSnapshotManagerPrivate(dpy, cbi)))
{
}

void WebOSAppSnapshotManager::onAboutToBlock()
{
    Q_D(WebOSAppSnapshotManager);

    d->initializeWaylandIntegration();
    d->disconnectToEventDispatcher(QGuiApplicationPrivate::eventDispatcher);
}
