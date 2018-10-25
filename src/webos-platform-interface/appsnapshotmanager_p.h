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

#ifndef APPSNAPSHOTMANAGER_P_H
#define APPSNAPSHOTMANAGER_P_H

#include "appsnapshotmanager.h"

#include <QString>
#include <QtCore/private/qobject_p.h>

class QWaylandDisplay;
class QWaylandEglClientBufferIntegration;
class QWindow;
class AppSnapshotManager;

class AppSnapshotManagerPrivate: public QObjectPrivate
{
public:
    Q_DECLARE_PUBLIC(AppSnapshotManager)

    AppSnapshotManagerPrivate();
    virtual ~AppSnapshotManagerPrivate();

    bool initialize(const QString& appId, QWindow* w, const int argc, char** argv);
    bool dump();
    void setAppSnapShotState(const AppSnapshotManager::AppSnapShotState state);
    void setAppSnapShotState(const AppSnapshotManager::AppSnapShotState state, const QString reason);

    bool preDump();
    bool postDump();
    bool restore();

    virtual bool preDumpInternal();
    virtual bool postDumpInternal();
    virtual bool restoreInternal();

    AppSnapshotManager::AppSnapShotState m_state;
    QString m_failureReason;
    QString m_workingPath;
    QString m_appId;
    QWindow* m_window;
    int m_argc;
    char** m_argv;
    bool m_cmdlineArgsChangedPending;
};

#endif
