// Copyright (c) 2013-2019 LG Electronics, Inc.
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

#ifndef WEBOSSHELLSURFACE_H
#define WEBOSSHELLSURFACE_H

#include <QObject>
#include <QWindow>
#include <QPointF>

class QPlatformWindow;
class WebOSShellSurfacePrivate;

struct wl_shell_surface;
struct wl_webos_shell_surface;

class WebOSShellSurface : public QObject {
    Q_OBJECT
    Q_DECLARE_PRIVATE(WebOSShellSurface)

    Q_PROPERTY(QPointF position READ position NOTIFY positionChanged)

public:

    enum LocationHint {
        LocationHintUnknown = 0,
        LocationHintNorth   = 1,
        LocationHintWest    = 2,
        LocationHintSouth   = 4,
        LocationHintEast    = 8,
        LocationHintCenter  = 16
    };
    Q_DECLARE_FLAGS(LocationHints, LocationHint)

    enum KeyMask {
        KeyMaskHome                 = 1,
        KeyMaskBack                 = 1 << 1,
        KeyMaskExit                 = 1 << 2,
        KeyMaskLeft                 = 1 << 3,
        KeyMaskRight                = 1 << 4,
        KeyMaskUp                   = 1 << 5,
        KeyMaskDown                 = 1 << 6,
        KeyMaskOk                   = 1 << 7,
        KeyMaskNumeric              = 1 << 8,
        KeyMaskRemoteColorRed       = 1 << 9,
        KeyMaskRemoteColorGreen     = 1 << 10,
        KeyMaskRemoteColorYellow    = 1 << 11,
        KeyMaskRemoteColorBlue      = 1 << 12,
        KeyMaskRemoteProgrammeGroup = 1 << 13,
        KeyMaskRemotePlaybackGroup  = 1 << 14,
        KeyMaskRemoteTeletextGroup  = 1 << 15,
        KeyMaskLocalLeft            = 1 << 16,
        KeyMaskLocalRight           = 1 << 17,
        KeyMaskLocalUp              = 1 << 18,
        KeyMaskLocalDown            = 1 << 19,
        KeyMaskLocalOk              = 1 << 20,
        KeyMaskRemoteMagnifierGroup = 1 << 21,
        KeyMaskMinimalPlaybackGroup = 1 << 22,
        KeyMaskGuide                = 1 << 23,
        KeyMaskDefault = 0xFFFFFFF8
    };
    Q_DECLARE_FLAGS(KeyMasks, KeyMask)

    virtual ~WebOSShellSurface();

    QPointF position();

#if (QT_VERSION < QT_VERSION_CHECK(5,10,0))
    void setState(Qt::WindowState state);
#else
    void setState(Qt::WindowStates state);
#endif

    QVariantMap properties() const;
    QVariant property(const QString &name) const;
    QVariant property(const QString &name, const QVariant &defaultValue) const;
    void setProperty(const QString &name, const QVariant &value);

    LocationHints locationHint();
    void setLocationHint(LocationHints hint);

    KeyMasks keyMask();
    void setKeyMask(KeyMasks keyMask);
    void setInputRegion(const QRegion& region);

signals:
    void positionChanged();
    void stateAboutToChange(Qt::WindowState state);
    void locationHintChanged();
    void keyMaskChanged();

private:
    WebOSShellSurface(wl_webos_shell_surface* s, struct ::wl_shell_surface *ss, QPlatformWindow* parent);

    void emitPositionChanged();
    void emitStateAboutToChange(Qt::WindowState state);
    void emitLocationHintChanged();
    void emitKeyMaskChanged();

    WebOSShellSurfacePrivate *d_ptr;
    Q_DISABLE_COPY(WebOSShellSurface);

    friend class WebOSShellPrivate;

};

Q_DECLARE_OPERATORS_FOR_FLAGS(WebOSShellSurface::LocationHints)
Q_DECLARE_OPERATORS_FOR_FLAGS(WebOSShellSurface::KeyMasks)

#endif
