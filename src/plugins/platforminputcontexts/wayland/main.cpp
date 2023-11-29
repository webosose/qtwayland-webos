// Copyright (c) 2013-2018 LG Electronics, Inc.
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

#include <qpa/qplatforminputcontextplugin_p.h>
#include <QtCore/QStringList>
#include "waylandinputcontext.h"

QT_BEGIN_NAMESPACE

class WaylandPlatformInputContextPlugin : public QPlatformInputContextPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QPlatformInputContextFactoryInterface.5.1" FILE "wayland.json")

public:
    QPlatformInputContext *create(const QString&, const QStringList&);
};

QPlatformInputContext* WaylandPlatformInputContextPlugin::create(const QString& system, const QStringList& paramList)
{
    Q_UNUSED(paramList);
    if (system.compare(system, QStringLiteral("wayland"), Qt::CaseInsensitive) == 0
        || system.compare(system, QStringLiteral("wayland-webos"), Qt::CaseInsensitive) == 0)
        return new WaylandInputContext;
    return 0;
}

QT_END_NAMESPACE

#include "main.moc"
