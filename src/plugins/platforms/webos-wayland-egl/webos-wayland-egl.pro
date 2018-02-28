# Copyright (c) 2015-2018 LG Electronics, Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# SPDX-License-Identifier: Apache-2.0

PLUGIN_TYPE = platforms
load(qt_plugin)

QT += waylandclient-private

INCLUDEPATH += ../../../webos-platform-interface
LIBS += -L../../../webos-platform-interface -lwebos-platform-interface

OTHER_FILES += \
    webos-wayland-egl.json

SOURCES += \
    main.cpp \
    webosintegration.cpp \
    webosplatformwindow.cpp \
    webosnativeinterface.cpp \
    weboscursor.cpp \
    webosinputdevice.cpp \
    webosscreen.cpp

HEADERS += \
    weboseglplatformintegration.h \
    webosintegration_p.h \
    webosplatformwindow_p.h \
    webosnativeinterface_p.h \
    weboscursor_p.h \
    webosinputdevice_p.h \
    webosscreen_p.h \
    qtwaylandwebostrace.h

lttng {
    DEFINES += HAS_LTTNG
    SOURCES +=  pmtrace_qtwaylandwebos_provider.c
    HEADERS +=  pmtrace_qtwaylandwebos_provider.h
    !contains(QT_CONFIG, no-pkg-config) {
        CONFIG += link_pkgconfig
        PKGCONFIG += lttng-ust
    } else {
        LIBS += -llttng-ust
    }
}
