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

set(re-common_MAJOR_VERSION 3)
set(re-common_MINOR_VERSION 2)
set(re-common_PATCH_VERSION 1)
set(re-common_VERSION "${re-common_MAJOR_VERSION}.${re-common_MINOR_VERSION}.${re-common_PATCH_VERSION}")

# Defines the location of the sources
set(RE_COMMON_CPP_SRC_DIR "${CMAKE_CURRENT_LIST_DIR}/src/cpp")

include("${CMAKE_CURRENT_LIST_DIR}/cmake/RECommonFetch_re-logging.cmake")

# The provided JukeboxExports only create the main Device, so if you have a more complex case, involving
# native objects, you should set this to OFF
option(RE_COMMON_PROVIDES_JUKEBOX_EXPORTS "Set to OFF is your RE uses native objects" ON)

# Defines the sources
set(re-common_BUILD_SOURCES
    ${RE_COMMON_CPP_SRC_DIR}/AudioSocket.cpp
    ${RE_COMMON_CPP_SRC_DIR}/JBoxProperty.cpp
    ${RE_COMMON_CPP_SRC_DIR}/JBoxPropertyManager.cpp
    ${RE_COMMON_CPP_SRC_DIR}/jbox.cpp
  )

if(RE_COMMON_PROVIDES_JUKEBOX_EXPORTS)
  list(APPEND re-common_BUILD_SOURCES "${RE_COMMON_CPP_SRC_DIR}/JukeboxExports.cpp")
endif()

# Defines the headers if you want to include them in your project (optional)
set(re-common_BUILD_HEADERS
    ${RE_COMMON_CPP_SRC_DIR}/AudioBuffer.h
    ${RE_COMMON_CPP_SRC_DIR}/AudioSocket.h
    ${RE_COMMON_CPP_SRC_DIR}/CVSocket.h
    ${RE_COMMON_CPP_SRC_DIR}/fmt.h
    ${RE_COMMON_CPP_SRC_DIR}/jbox.h
    ${RE_COMMON_CPP_SRC_DIR}/CircularBuffer.h
    ${RE_COMMON_CPP_SRC_DIR}/CommonDevice.h
    ${RE_COMMON_CPP_SRC_DIR}/Constants.h
    ${RE_COMMON_CPP_SRC_DIR}/JBoxProperty.h
    ${RE_COMMON_CPP_SRC_DIR}/JBoxPropertyManager.h
    ${RE_COMMON_CPP_SRC_DIR}/JukeboxExports.h
    ${RE_COMMON_CPP_SRC_DIR}/Utils.h
    ${RE_COMMON_CPP_SRC_DIR}/SampleRateBasedClock.h
    ${RE_COMMON_CPP_SRC_DIR}/StaticString.h
    ${RE_COMMON_CPP_SRC_DIR}/Volume.h
    ${RE_COMMON_CPP_SRC_DIR}/XFade.h
    ${RE_COMMON_CPP_SRC_DIR}/pongasoft/common/StaticVector.hpp
    ${RE_COMMON_CPP_SRC_DIR}/pongasoft/common/stl.h
  )

# Define the sources for native build only
set(re-common_NATIVE_BUILD_SOURCES
    ${re-logging_SOURCES}
)


# Defines the include directories
set(re-common_INCLUDE_DIRECTORIES "${RE_COMMON_CPP_SRC_DIR}" "${re-logging_INCLUDE_DIRS}")

