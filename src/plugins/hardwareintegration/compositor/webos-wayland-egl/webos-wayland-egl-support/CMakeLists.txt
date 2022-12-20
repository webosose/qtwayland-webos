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
## WebOSEglClientBuffer Module:
#####################################################################

# special case begin
find_package(Qt6 REQUIRED COMPONENTS WaylandEglCompositorHwIntegrationPrivate)

qt_internal_add_module(WebOSEglClientBuffer
    CONFIG_MODULE_NAME webos_egl_clientbuffer
    SOURCES
        weboseglclientbufferintegration.cpp weboseglclientbufferintegration_p.h
    LIBRARIES
        Qt::CorePrivate
        Qt::GuiPrivate
        Qt::WaylandCompositorPrivate
        Qt::WaylandEglCompositorHwIntegrationPrivate
    PUBLIC_LIBRARIES
        Qt::Core
        Qt::Gui
        Qt::WaylandCompositor
    PRIVATE_MODULE_INTERFACE
        Qt::CorePrivate
        Qt::GuiPrivate
        Qt::WaylandCompositorPrivate
)

#### Keys ignored in scope 1:.:.:webos-wayland-egl-support.pro:<TRUE>:
# MODULE = "webos_egl_clientbuffer"

## Scopes:
#####################################################################

# special case end
