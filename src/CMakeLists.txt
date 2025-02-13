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

add_subdirectory(webos-platform-interface)
add_subdirectory(plugins)

# webos-platform-interface must be built first
add_dependencies(WaylandPlatformInputContextPlugin webos-platform-interface)
add_dependencies(webos-wayland-egl webos-platform-interface)
add_dependencies(WebOSWaylandWLShellIntegrationPlugin webos-platform-interface)
