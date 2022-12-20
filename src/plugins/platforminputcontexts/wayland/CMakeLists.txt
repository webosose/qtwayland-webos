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
## WaylandPlatformInputContextPlugin Plugin:
#####################################################################

# special case begin
pkg_check_modules(WAYLAND_WEBOS_CLIENT REQUIRED wayland-webos-client)

qt_internal_add_plugin(WaylandPlatformInputContextPlugin
    OUTPUT_NAME waylandplatforminputcontextplugin
    PLUGIN_TYPE platforminputcontexts
    SOURCES
        keysymhelper.h
        main.cpp
        waylandinputcontext.cpp waylandinputcontext.h
    INCLUDE_DIRECTORIES
        ../../../webos-platform-interface
    LIBRARIES
        Qt::GuiPrivate
    PUBLIC_LIBRARIES
        Qt::Core
        Qt::Gui
        ${WAYLAND_WEBOS_CLIENT_LIBRARIES}
        ${CMAKE_BINARY_DIR}/lib/libwebos-platform-interface.so
)

#### Keys ignored in scope 1:.:.:wayland.pro:<TRUE>:
# MOC_DIR = ".moc"
# OBJECTS_DIR = ".obj"
# PKGCONFIG = "wayland-webos-client" "xkbcommon"

# special case end
