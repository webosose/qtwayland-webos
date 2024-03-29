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
## webos Plugin:
#####################################################################

# special case begin
find_package(Qt6 REQUIRED COMPONENTS WlShellIntegrationPrivate)

qt_internal_add_plugin(WebOSWaylandWLShellIntegrationPlugin
    OUTPUT_NAME webos
    PLUGIN_TYPE wayland-shell-integration
    SOURCES
        main.cpp
    INCLUDE_DIRECTORIES
        ../../../webos-platform-interface
    LIBRARIES
        Qt::WaylandClientPrivate
        Qt::WlShellIntegrationPrivate
        ${CMAKE_BINARY_DIR}/lib/libwebos-platform-interface.so
)

#### Keys ignored in scope 1:.:.:webos.pro:<TRUE>:
# OTHER_FILES = "webos.json"

# special case end
