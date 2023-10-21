# Copyright (c) 2023 LG Electronics, Inc.
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
## webos-platform-interface Generic Library:
#####################################################################

# special case begin
set(RECIPE_SYSROOT "$ENV{PKG_CONFIG_SYSROOT_DIR}")

set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS
    WaylandCompositor
    WaylandScannerTools
    WlShellIntegrationPrivate
)

pkg_check_modules(WaylandWebOSClient REQUIRED wayland-webos-client IMPORTED_TARGET)

if (NOT WaylandScanner_FOUND OR NOT Wayland_FOUND)
    message(WARNING "QtWayland is missing required dependencies, nothing will be built. \
Although this could be considered an error, the configuration will still pass as coin (Qt's \
continuous integration system) will fail the build if configure fails, but will still try to \
configure the module on targets that are missing dependencies.")
    return()
endif()

include(${RECIPE_SYSROOT}/usr/lib/cmake/Qt6WaylandClient/Qt6WaylandClientMacros.cmake)

set(HEADERS
    webosforeign.h
    webosinputmanager.h
    webosinputpanellocator.h
    webosplatform.h
    webospresentationtime.h
    webosshell.h
    webosshellintegration.h
    webosshellsurface.h
    webossurfacegroup.h
    webossurfacegroupcompositor.h
    webossurfacegrouplayer.h
    webostablet.h
    webosxinput.h
)

set (PRIVATE_HEADERS
    webosforeign_p.h
    webosinputmanager_p.h
    webosinputpanellocator_p.h
    webosplatform_p.h
    webospresentationtime_p.h
    webosshellsurface_p.h
    webossurfacegroup_p.h
    webossurfacegroupcompositor_p.h
    webossurfacegrouplayer_p.h
    webostablet_p.h
)

set (SOURCES
    webosforeign.cpp
    webosinputmanager.cpp
    webosinputpanellocator.cpp
    webosplatform.cpp
    webospresentationtime.cpp
    webosshell.cpp
    webosshellintegration.cpp
    webosshellsurface.cpp
    webossurfacegroup.cpp
    webossurfacegroupcompositor.cpp
    webossurfacegrouplayer.cpp
    webostablet.cpp
    webosxinput.cpp
)

if(FEATURE_criu)
    list(APPEND SOURCES appsnapshotmanager.cpp)
    list(APPEND HEADERS appsnapshotmanager.h)
    list(APPEND PRIVATE_HEADERS appsnapshotmanager_p.h)
endif()

qt_internal_add_cmake_library(webos-platform-interface
    SOURCES
        ${SOURCES}
        ${HEADERS}
        ${PRIVATE_HEADERS}
    LIBRARIES
        Qt::CorePrivate
        Qt::GuiPrivate
        Qt::WaylandClientPrivate
        Qt::WaylandCompositorPrivate
        Qt::WlShellIntegrationPrivate
    PUBLIC_LIBRARIES
        PkgConfig::WaylandWebOSClient
        Qt::Core
        Qt::Gui
        Wayland::Client
    INCLUDE_DIRECTORIES
        ../../include
)

qt_generate_wayland_protocol_client_sources(webos-platform-interface
    FILES
        ${RECIPE_SYSROOT}/usr/share/wayland-protocols/stable/presentation-time/presentation-time.xml
        ${RECIPE_SYSROOT}/usr/share/wayland-webos/webos-extension.xml
        ${RECIPE_SYSROOT}/usr/share/wayland-webos/webos-foreign.xml
        ${RECIPE_SYSROOT}/usr/share/wayland-webos/webos-input-manager.xml
        ${RECIPE_SYSROOT}/usr/share/wayland-webos/webos-surface-group.xml
        ${RECIPE_SYSROOT}/usr/share/wayland-webos/webos-tablet.xml
)

#### Keys ignored in scope 1:.:.:webos-platform-interface.pro:<TRUE>:
# INSTALLS = "headers" "target"
# PKGCONFIG = "wayland-webos-client"
# QMAKE_CLEAN = "lib$$TARGET.prl"
# QMAKE_PKGCONFIG_DESCRIPTION = "The" "native" "Qt" "interface" "for" "webos" "clients"
# QMAKE_PKGCONFIG_DESTDIR = "pkgconfig"
# QMAKE_PKGCONFIG_INCDIR = "$$header.path"
# QMAKE_PKGCONFIG_LIBDIR = "$$target.path"
# QMAKE_PKGCONFIG_NAME = "webos-platform-interface"
# TEMPLATE = "lib"
# headers.files = "$$PUBLICHEADERS"
# headers.path = "$$WEBOS_INSTALL_HEADERS"
# target.path = "$$[WEBOS_INSTALL_LIBS]"

set(SO_VERSION 1)
set(VERSION_FULL ${SO_VERSION}.0.0)

set_target_properties(webos-platform-interface PROPERTIES
    VERSION ${VERSION_FULL}
    SOVERSION ${SO_VERSION}
    C_VISIBILITY_PRESET "default"
    CXX_VISIBILITY_PRESET "default"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)

add_custom_command(TARGET webos-platform-interface POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    $<TARGET_FILE:webos-platform-interface>
    "${CMAKE_BINARY_DIR}/lib/libwebos-platform-interface.so.${VERSION_FULL}"
    COMMAND ${CMAKE_COMMAND} -E create_symlink
    "libwebos-platform-interface.so.${VERSION_FULL}"
    "${CMAKE_BINARY_DIR}/lib/libwebos-platform-interface.so.${SO_VERSION}"
    VERBATIM)

## Scopes:
#####################################################################

#### Keys ignored in scope 4:.:.:webos-platform-interface.pro:criu:
# PKGCONFIG = "criu"

qt_internal_extend_target(webos-platform-interface CONDITION FEATURE_criu
    DEFINES
        HAS_CRIU
    LIBRARIES
        PkgConfig::CRIU
)

configure_file(webos-platform-interface.pc.in webos-platform-interface.pc @ONLY)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/webos-platform-interface.pc
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
    COMPONENT Development
)

install(FILES
    ${HEADERS}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    COMPONENT Development
)

install(FILES
    ${CMAKE_BINARY_DIR}/lib/libwebos-platform-interface.so.${VERSION_FULL}
    ${CMAKE_BINARY_DIR}/lib/libwebos-platform-interface.so.${SO_VERSION}
    ${CMAKE_BINARY_DIR}/lib/libwebos-platform-interface.so
    DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
# special case end
