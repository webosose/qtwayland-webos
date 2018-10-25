# Copyright (c) 2013-2018 LG Electronics, Inc.
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

TEMPLATE = lib

QT += gui-private core-private waylandclient-private

CONFIG += link_pkgconfig wayland-scanner
PKGCONFIG += wayland-webos-client wayland-client

WAYLANDCLIENTSOURCES += $$[QT_INSTALL_PREFIX]/share/wayland-webos/webos-surface-group.xml
WAYLANDCLIENTSOURCES += $$[QT_INSTALL_PREFIX]/share/wayland-webos/webos-input-manager.xml
WAYLANDCLIENTSOURCES += $$[QT_INSTALL_PREFIX]/share/wayland-webos/webos-extension.xml

SOURCES += \
    webosshellintegration.cpp \
    webosplatform.cpp \
    webosshell.cpp \
    webosshellsurface.cpp \
    webossurfacegroupcompositor.cpp \
    webossurfacegroup.cpp \
    webossurfacegrouplayer.cpp \
    webosxinput.cpp \
    webosinputmanager.cpp \
    webosinputpanellocator.cpp

PUBLICHEADERS = \
    webosplatform.h \
    webosshell.h \
    webosshellsurface.h \
    webossurfacegroupcompositor.h \
    webossurfacegroup.h \
    webossurfacegrouplayer.h \
    webosinputmanager.h \
    webosxinput.h \
    webosshellintegration.h \
    webosinputpanellocator.h

HEADERS += \
    $$PUBLICHEADERS \
    webosplatform_p.h \
    webosshell_p.h \
    webosshellsurface_p.h \
    webossurfacegroupcompositor_p.h \
    webossurfacegroup_p.h \
    webossurfacegrouplayer_p.h \
    webosinputmanager_p.h \
    webosinputpanellocator_p.h

criu {
    DEFINES += HAS_CRIU
    PKGCONFIG += criu
    SOURCES += appsnapshotmanager.cpp
    PUBLICHEADERS += appsnapshotmanager.h
    HEADERS += appsnapshotmanager_p.h
}

headers.files += $$PUBLICHEADERS

headers.path += $$WEBOS_INSTALL_HEADERS

target.path = $$WEBOS_INSTALL_LIBS

CONFIG += create_pc create_prl no_install_prl
QMAKE_PKGCONFIG_NAME = webos-platform-interface
QMAKE_PKGCONFIG_DESCRIPTION = The native Qt interface for webos clients
QMAKE_PKGCONFIG_LIBDIR = $$target.path
QMAKE_PKGCONFIG_INCDIR = $$header.path
QMAKE_PKGCONFIG_DESTDIR = pkgconfig

INSTALLS += headers target

#To build plugins in qtwayland-webos
#QMAKE_POST_LINK = cp $$OUT_PWD/*.so $$PWD/../../libs
#QMAKE_POST_LINK = make install

QMAKE_CLEAN += lib$$TARGET.prl
