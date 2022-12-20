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

cmake_minimum_required(VERSION 3.16)

include(.cmake.conf)
project(wayland-webos
    VERSION "${QT_REPO_MODULE_VERSION}"
    DESCRIPTION "Qt wayland-webos Libraries"
    HOMEPAGE_URL "https://github.com/webosose/qtwayland-webos"
    LANGUAGES CXX C
)

find_package(Qt6 REQUIRED COMPONENTS
    BuildInternals
    Core
    Gui
    WaylandClient
    WaylandCompositor
)

find_package(CRIU)

qt_build_repo()
