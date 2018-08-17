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

#include "appsnapshotmanager.h"
#include "appsnapshotmanager_p.h"

#include <QWindow>
#include <QDebug>
#include <QCoreApplication>

#include <criue.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

static char** dup_commmand_line_argv(const int argc, char** argv)
{
    Q_ASSERT(argc>0);
    Q_ASSERT(argv);

    char** dup_argv = new char*[argc];
    Q_ASSERT(dup_argv);

    for (int i = 0; i < argc; ++i) {
        if (!argv[i])
            continue;

        dup_argv[i] = new char[strlen(argv[i])+1];
        Q_ASSERT(dup_argv[i]);

        strcpy(dup_argv[i], argv[i]);
    }

    return dup_argv;
}

static void free_command_line_arguments(int* argc, char*** argv)
{
    Q_ASSERT(*argc>0);
    Q_ASSERT(*argv);

    int p_argc = *argc;
    char** p_argv = *argv;
    for (int i = 0; i < p_argc; ++i) {
        if (!p_argv[i])
            continue;

        delete [] p_argv[i];
        p_argv[i] = NULL;
    }
    delete [] p_argv;

    *argc = 0;
    *argv = NULL;
}

static QString to_qstring(const int argc, char** argv)
{
    Q_ASSERT(argc>0);
    Q_ASSERT(argv);

    QString str;
    for (int i = 0; i < argc; ++i) {
        if (!argv[i])
            continue;

        str.append(QString::fromUtf8(argv[i]));
    }

    return str;
}

static int pre_dump(void* data)
{
    Q_ASSERT(data);

    bool rv = static_cast<AppSnapshotManagerPrivate*>(data)->preDump();

    int fd = open("/dev/null", O_RDWR);
    if (fd != -1) {
        dup2(fd, 0);
        dup2(fd, 1);
        dup2(fd, 2);

        int err = setsid();
        if (err < 0) {
            qWarning() << "failure to setsid() function : error=" << strerror(err);
            rv = false;
        }
        close(fd);
    }

    return rv ? 0 : -1;
}

static int post_dump(void* data)
{
    Q_ASSERT(data);
    return static_cast<AppSnapshotManagerPrivate*>(data)->postDump() ? 0 : -1;
}

static int restore(void* data)
{
    Q_ASSERT(data);
    return static_cast<AppSnapshotManagerPrivate*>(data)->restore() ? 0 : -1;
}

QDebug operator<<(QDebug debug, const AppSnapshotManager::AppSnapShotState state)
{
    QDebugStateSaver saver(debug);
    switch (state) {
        case AppSnapshotManager::None:
            debug.nospace() << "(None)";
            break;
        case AppSnapshotManager::Initialized:
            debug.nospace() << "(Initialized)";
            break;
        case AppSnapshotManager::Ready:
            debug.nospace() << "(Ready)";
            break;
        case AppSnapshotManager::PreDumped:
            debug.nospace() << "(PreDumped)";
            break;
        case AppSnapshotManager::Dumped:
            debug.nospace() << "(Dumped)";
            break;
        case AppSnapshotManager::PostDumped:
            debug.nospace() << "(PostDumped)";
            break;
        case AppSnapshotManager::Restored:
            debug.nospace() << "(Restored)";
            break;
        case AppSnapshotManager::Failure:
            debug.nospace() << "(Failure)";
            break;
        default:
            break;
    }

    return debug;
}

AppSnapshotManagerPrivate::AppSnapshotManagerPrivate()
    : QObjectPrivate()
    , m_state(AppSnapshotManager::None)
    , m_window(NULL)
    , m_argc(0)
    , m_argv(NULL)
    , m_cmdlineArgsChangedPending(false)
{
}

AppSnapshotManagerPrivate::~AppSnapshotManagerPrivate()
{
    if (m_argv)
        free_command_line_arguments(&m_argc, &m_argv);
}

bool AppSnapshotManagerPrivate::initialize(const QString& appId, QWindow* w, const int argc, char** argv)
{
    Q_ASSERT(appId.size());
    Q_ASSERT(w);
    Q_ASSERT(argc>0);
    Q_ASSERT(argv);

    if (m_argv)
        free_command_line_arguments(&m_argc, &m_argv);

    criue_init_opts();
    criue_set_service_address(NULL);

    criue_set_appid(appId.toLocal8Bit().data());
    criue_set_comm(QCoreApplication::applicationName().toLocal8Bit().data());
    criue_set_pid(getpid());
    criue_set_shell_job(true);
    criue_set_ext_unix_sk(true);
    criue_set_leave_running(true);
    criue_set_checkpoint_cb(pre_dump);
    criue_set_restore_cb(::restore);
    criue_set_postdump_cb(post_dump);
    criue_set_context(this);

    m_appId = appId;
    m_window = w;
    m_argc = argc;
    m_argv = dup_commmand_line_argv(argc, argv);
    m_cmdlineArgsChangedPending = false;
    setAppSnapShotState(AppSnapshotManager::Initialized);

    qDebug() << "initialization to AppSnapshotManager: "
             << "appId=" << m_appId << ", "
             << "window=" << m_window << ", "
             << "state=" << m_state << ", "
             << "argc=" << m_argc << ", "
             << "argv=" << to_qstring(m_argc, m_argv);

    return true;
}

bool AppSnapshotManagerPrivate::dump()
{
    Q_Q(AppSnapshotManager);

    if (m_state != AppSnapshotManager::Initialized) {
        qWarning() << "disallow to call dump() function again: "
                   << "state=" << m_state << ", "
                   << "reason=" << m_failureReason;
        return q->isPostDumped() || q->isRestored();
    }

    // criue_dump() is a start-point to dump a snapshot of this process,
    // and also is a entry-point to restore a process from the dumped snapshot.
    // if failing of the criue_dump() function, we just do postDump() function
    // regardless of whether the process would be restored from the dumped snapshot or not.
    int rv = criue_dump();
    if (rv < 0 || !(q->isPostDumped() || q->isRestored())) {
        qWarning() << "fail in cirue_dump function: "
                   << "errno=" << strerror(rv) << ", "
                   << "state=" << m_state << ", "
                   << "reason=" << m_failureReason;

        AppSnapshotManager::AppSnapShotState state;
        QString failureReason;
        if (m_state == AppSnapshotManager::Failure && m_failureReason.size()) {
            // if-condition indicates that the reason failed to criue_dump() is Qt framework.
            // we keeps correct state
            state = m_state;
            failureReason = m_failureReason;
        }
        else {
            // else-condition indicates that the reason failing to criue_dump() is criue library.
            // we set the state to Failure
            state = AppSnapshotManager::Failure;
            failureReason = "dump";
        }

        if (!postDump())
            qWarning() << "failure of postdump: "
                       << "state=" << m_state << ", "
                       << "reason=" << m_failureReason;
        m_state = state;
        m_failureReason = failureReason;
        m_window->setScreen(NULL);

        return false;
    }
    Q_ASSERT(q->isPostDumped() || q->isRestored());

    m_window->setScreen(NULL);
    if (m_cmdlineArgsChangedPending) {
        emit q->commandLineArgumentsChanged(m_argc, m_argv);
        m_cmdlineArgsChangedPending = false;
    }
    qDebug() << "success to dump or restore for app snapshot: state=" << m_state;

    return true;
}

void AppSnapshotManagerPrivate::setAppSnapShotState(const AppSnapshotManager::AppSnapShotState state)
{
    setAppSnapShotState(state, "");
}

void AppSnapshotManagerPrivate::setAppSnapShotState(const AppSnapshotManager::AppSnapShotState state, const QString reason)
{
    if (m_state != state || m_failureReason != reason) {
        Q_Q(AppSnapshotManager);

        m_state = state;
        m_failureReason = reason;
        qDebug() << "AppSnapshotState is changed: "
                 << "state=" << m_state << ", "
                 << "reason=" << m_failureReason;

        emit q->stateChanged(m_state, m_failureReason);
    }
}

bool AppSnapshotManagerPrivate::preDump()
{
    qDebug() << Q_FUNC_INFO;

    setAppSnapShotState(AppSnapshotManager::Ready);

    if (!preDumpInternal()) {
        qWarning() << "failure of predump";
        setAppSnapShotState(AppSnapshotManager::Failure, "predump");
        return false;
    }

    setAppSnapShotState(AppSnapshotManager::PreDumped);

    return true;
}

bool AppSnapshotManagerPrivate::postDump()
{
    qDebug() << Q_FUNC_INFO;

    setAppSnapShotState(AppSnapshotManager::Dumped);

    if (!postDumpInternal()) {
        qWarning() << "failure of postdump";
        setAppSnapShotState(AppSnapshotManager::Failure, "postdump");
        return false;
    }

    setAppSnapShotState(AppSnapshotManager::PostDumped);

    return true;
}

bool AppSnapshotManagerPrivate::restore()
{
    qDebug() << Q_FUNC_INFO;

    setAppSnapShotState(AppSnapshotManager::Dumped);

    if (!restoreInternal()) {
        qWarning() << "failure of restore";
        setAppSnapShotState(AppSnapshotManager::Failure, "restore");
        return false;
    }

    setAppSnapShotState(AppSnapshotManager::Restored);

    return true;
}

bool AppSnapshotManagerPrivate::preDumpInternal()
{
    return true;
}

bool AppSnapshotManagerPrivate::postDumpInternal()
{
    return true;
}

bool AppSnapshotManagerPrivate::restoreInternal()
{
    Q_ASSERT(m_window);
    Q_ASSERT(m_argc>0);
    Q_ASSERT(m_argv);

    // get commandline arguments
    int argc = m_argc;
    char** argv = dup_commmand_line_argv(m_argc, m_argv);
    if (criue_get_argument(&argc, &argv) < 0) {
        free_command_line_arguments(&argc, &argv);
        qWarning() << "fail in getting arguments";
        return false;
    }
    Q_ASSERT(argc);
    Q_ASSERT(argv);

    if (argc!=m_argc || to_qstring(argc, argv)!=to_qstring(m_argc, m_argv)) {
        free_command_line_arguments(&m_argc, &m_argv);

        m_cmdlineArgsChangedPending = true;
        m_argc = argc;
        m_argv = dup_commmand_line_argv(argc, argv);
    }
    free_command_line_arguments(&argc, &argv);

    return true;
}

AppSnapshotManager::AppSnapshotManager()
    : QObject(*new AppSnapshotManagerPrivate, 0)
{
}

AppSnapshotManager::AppSnapshotManager(AppSnapshotManagerPrivate& dd)
    : QObject(dd, 0)
{
}

bool AppSnapshotManager::initialize(const QString& appId, QWindow* w, const int argc, char** argv)
{
    Q_D(AppSnapshotManager);
    return d->initialize(appId, w, argc, argv);
}

bool AppSnapshotManager::dump()
{
    Q_D(AppSnapshotManager);
    return d->dump();
}

bool AppSnapshotManager::isPostDumped()
{
    Q_D(AppSnapshotManager);
    return (d->m_state == AppSnapshotManager::PostDumped);
}

bool AppSnapshotManager::isRestored()
{
    Q_D(AppSnapshotManager);
    return (d->m_state == AppSnapshotManager::Restored);
}
