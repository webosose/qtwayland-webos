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
## WebOSWaylandEglClientBufferIntegrationPlugin Plugin:
#####################################################################

# special case begin
find_package(Qt6 REQUIRED COMPONENTS
    WaylandCompositor
    WaylandEglCompositorHwIntegrationPrivate
)

qt_internal_add_plugin(WebOSWaylandEglClientBufferIntegrationPlugin
    OUTPUT_NAME webos-wayland-egl-plugin
    PLUGIN_TYPE wayland-graphics-integration-server
    SOURCES
        main.cpp
    LIBRARIES
        Qt::WaylandCompositorPrivate
        Qt::WaylandEglCompositorHwIntegrationPrivate
    PUBLIC_LIBRARIES
        Qt::WebOSEglClientBuffer
)

add_dependencies(WebOSWaylandEglClientBufferIntegrationPlugin WebOSEglClientBuffer)

#### Keys ignored in scope 1:.:.:webos-wayland-egl-plugin.pro:<TRUE>:
# OTHER_FILES = "webos-wayland-egl.json"

## Scopes:
#####################################################################

# special case end
