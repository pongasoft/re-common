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
# Git repo/tag for re-logging
#------------------------------------------------------------------------
set(re-logging_GIT_REPO "https://github.com/pongasoft/re-logging" CACHE STRING "re-logging git repository url")
set(re-logging_GIT_TAG "v1.0.1" CACHE STRING "re-logging git tag")

re_common_fetch_content(NAME re-logging)

include("${re-logging_ROOT_DIR}/re-logging.cmake")
