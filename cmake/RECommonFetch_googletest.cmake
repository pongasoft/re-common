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

#------------------------------------------------------------------------
# The git respository to fetch googletest from
#------------------------------------------------------------------------
set(googletest_GIT_REPO "https://github.com/google/googletest" CACHE STRING "googletest git repository URL")

#------------------------------------------------------------------------
# The git tag for googletest
# release-1.11.0 => e2239ee6043f73722e7aa812a459f54a28552929
#------------------------------------------------------------------------
set(googletest_GIT_TAG "e2239ee6043f73722e7aa812a459f54a28552929" CACHE STRING "googletest git tag")

re_common_fetch_content(NAME googletest)

# Prevent overriding the parent project's compiler/linker settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "Set by re-cmake" FORCE)

# Do not install GoogleTest!
option(INSTALL_GTEST "Enable installation of googletest. (Projects embedding googletest may want to turn this OFF.)" OFF)

# Add googletest directly to our build. This defines the gtest and gtest_main targets.
add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR} EXCLUDE_FROM_ALL)
