# Copyright (c) 2022 pongasoft
#
# Licensed under the Apache License, Version 2.0 (the "License"); you may not
# use this file except in compliance with the License. You may obtain a copy of
# the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
# License for the specific language governing permissions and limitations under
# the License.
#
# @author Yan Pujante

cmake_minimum_required(VERSION 3.17)

include("${CMAKE_CURRENT_LIST_DIR}/RECommonFetchContent.cmake")

set(googletest_GIT_REPO "https://github.com/google/googletest" CACHE STRING "googletest git repository URL")
set(googletest_GIT_TAG "v1.17.0" CACHE STRING "googletest git tag")
set(googletest_DOWNLOAD_URL "${googletest_GIT_REPO}/archive/refs/tags/${googletest_GIT_TAG}.zip" CACHE STRING "googletest download url" FORCE)
set(googletest_DOWNLOAD_URL_HASH "SHA256=40d4ec942217dcc84a9ebe2a68584ada7d4a33a8ee958755763278ea1c5e18ff" CACHE STRING "googletest download url hash" FORCE)

re_common_fetch_content(NAME googletest)

# Prevent overriding the parent project's compiler/linker settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "Set by re-common" FORCE)

# Do not install GoogleTest!
option(INSTALL_GTEST "Enable installation of googletest. (Projects embedding googletest may want to turn this OFF.)" OFF)

# Add googletest directly to our build. This defines the gtest and gtest_main targets.
add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR} EXCLUDE_FROM_ALL)
