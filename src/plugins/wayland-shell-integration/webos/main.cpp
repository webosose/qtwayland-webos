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

#include <QtWaylandClient/private/qwaylandshellintegrationfactory_p.h>
#include <QtWaylandClient/private/qwaylandshellintegrationplugin_p.h>

#include "webosshellintegration.h"

using QtWaylandClient::QWaylandShellIntegrationPlugin;

QT_BEGIN_NAMESPACE

class WebOSShellIntegrationPlugin : public QWaylandShellIntegrationPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QWaylandShellIntegrationFactoryInterface_iid FILE "webos.json")
public:
    QWaylandShellIntegration *create(const QString&, const QStringList&) override;
};

QWaylandShellIntegration *WebOSShellIntegrationPlugin::create(const QString& system, const QStringList& paramList)
{
    Q_UNUSED(paramList);
    Q_UNUSED(system);

    return new WebOSShellIntegration();
}

QT_END_NAMESPACE

#include "main.moc"
