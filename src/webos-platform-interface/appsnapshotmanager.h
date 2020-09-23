// Copyright (c) 2017-2020 LG Electronics, Inc.
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

#ifndef APPSNAPSHOTMANAGER_H
#define APPSNAPSHOTMANAGER_H

#include <QObject>
#include <functional>

class QString;
class QWindow;
class AppSnapshotManagerPrivate;

typedef std::function<void(void)> qttestability_callBackFunc;

class AppSnapshotManager : public QObject
{
    Q_OBJECT
public:
    enum AppSnapShotState {
        None = 0,
        Initialized,
        Ready,
        PreDumped,
        Dumped,
        PostDumped,
        Restored,
        Failure,
    };

    AppSnapshotManager();
    AppSnapshotManager(AppSnapshotManagerPrivate& dd);

    bool initialize(const QString& appId, QWindow* w, const int argc, char** argv);
    bool dump();
    bool isPostDumped();
    bool isRestored();
    static void qttestability_set_callback(qttestability_callBackFunc cb);

signals:
    void stateChanged(const AppSnapshotManager::AppSnapShotState state, const QString failureReason);
    void commandLineArgumentsChanged(const int argc, char** argv);

private:
    Q_DECLARE_PRIVATE(AppSnapshotManager)
    Q_DISABLE_COPY(AppSnapshotManager)
};

#endif
