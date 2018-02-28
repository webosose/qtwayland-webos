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

#ifndef WEBOSSURFACEGROUPLAYER_H
#define WEBOSSURFACEGROUPLAYER_H

#include <QObject>

class WebOSSurfaceGroupLayerPrivate;

class WebOSSurfaceGroupLayer : public QObject {

    Q_OBJECT

public:

    WebOSSurfaceGroupLayer();
    ~WebOSSurfaceGroupLayer();

    void setName(const QString& name) { m_name = name; }
    QString name() { return m_name; }

    void setZ(int z);
    int z() { return m_z; }

signals:
    void surfaceAttached();
    void surfaceDetached();

private:
    QScopedPointer<WebOSSurfaceGroupLayerPrivate> d_ptr;
    Q_DISABLE_COPY(WebOSSurfaceGroupLayer);
    Q_DECLARE_PRIVATE(WebOSSurfaceGroupLayer);

    QString m_name;
    int m_z;
};
#endif
