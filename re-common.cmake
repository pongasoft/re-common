# Copyright (c) 2020 pongasoft
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

# Defines the location of the sources
set(RE_COMMON_CPP_SRC_DIR "${CMAKE_CURRENT_LIST_DIR}/src/cpp")

message(STATUS "from re-common.cmake ${CMAKE_CURRENT_LIST_DIR}")

# Defines the sources
set(re-common_BUILD_SOURCES
    ${RE_COMMON_CPP_SRC_DIR}/AudioSocket.cpp
    ${RE_COMMON_CPP_SRC_DIR}/JBoxProperty.cpp
    ${RE_COMMON_CPP_SRC_DIR}/JBoxPropertyManager.cpp
    ${RE_COMMON_CPP_SRC_DIR}/JukeboxExports.cpp
  )

# Define the sources for native build only
set(re-common_NATIVE_BUILD_SOURCES
    ${RE_COMMON_CPP_SRC_DIR}/logging/loguru.cpp
)


# Defines the include directories
set(re-common_INCLUDE_DIRECTORIES "${RE_COMMON_CPP_SRC_DIR}")

