# Copyright (c) 2023-2025 LG Electronics, Inc.
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

#####################################################################
## webos-wayland-egl Plugin:
#####################################################################

# special case begin
find_package(Qt6 REQUIRED COMPONENTS
    WaylandEglClientHwIntegrationPrivate
    WlShellIntegrationPrivate
)

find_package(LTTngUST)

link_directories(${CMAKE_BINARY_DIR}/lib)

qt_internal_add_plugin(webos-wayland-egl
    OUTPUT_NAME webos-wayland-egl
    PLUGIN_TYPE platforms
    SOURCES
        main.cpp
        qtwaylandwebostracer.h
        weboscursor.cpp weboscursor_p.h
        weboseglplatformintegration.h
        webosinputdevice.cpp webosinputdevice_p.h
        webosintegration.cpp webosintegration_p.h
        webosnativeinterface.cpp webosnativeinterface_p.h
        webosplatformwindow.cpp webosplatformwindow_p.h
        webosscreen.cpp webosscreen_p.h
    INCLUDE_DIRECTORIES
        ../../../webos-platform-interface
        ../../../../include
    LIBRARIES
        Qt::WaylandClientPrivate
        Qt::WaylandEglClientHwIntegrationPrivate
        Qt::WlShellIntegrationPrivate
    PUBLIC_LIBRARIES
        Qt::Core
        Qt::Gui
        libwebos-platform-interface.so
)

#### Keys ignored in scope 1:.:.:webos-wayland-egl.pro:<TRUE>:
# OTHER_FILES = "webos-wayland-egl.json"

## Scopes:
#####################################################################

qt_internal_extend_target(webos-wayland-egl CONDITION QT_FEATURE_xkbcommon
    LIBRARIES
        XKB::XKB
)

qt_internal_extend_target(webos-wayland-egl CONDITION FEATURE_criu
    SOURCES
        webosappsnapshotmanager.cpp webosappsnapshotmanager.h
    DEFINES
        HAS_CRIU
    LIBRARIES
        PkgConfig::CRIU
)

qt_internal_extend_target(webos-wayland-egl CONDITION FEATURE_lttng
    SOURCES
        pmtrace_qtwaylandwebos_provider.c pmtrace_qtwaylandwebos_provider.h
    DEFINES
        HAS_LTTNG
        # qtwayland-webos/2.0.0-82-r18/recipe-sysroot/usr/include/urcu/arch/generic.h:192:13: error: 'CLOCK_MONOTONIC' undeclared (first use in this function)
        #   192 |         if (caa_unlikely(clock_gettime(CLOCK_MONOTONIC, &ts)))
        _POSIX_C_SOURCE=199309L
    PUBLIC_LIBRARIES
        LTTng::UST
)

#### Keys ignored in scope 7:.:.:webos-wayland-egl.pro:NOT QT_CONFIG___contains___no-pkg-config:
# PKGCONFIG = "lttng-ust"

# special case end
