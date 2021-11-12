/*
 * Copyright (c) 2020 pongasoft
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 *
 * @author Yan Pujante
 */

#pragma once

#ifndef __PongasoftCommon_logging_h__
#define __PongasoftCommon_logging_h__


// Implementation note: when doing jbox builds, loguru.cpp cannot be compiled because it relies on includes
// (like atomic) which are not part of the RE SDK due to sandboxing, as a result we disable logging in this instance
#if LOCAL_NATIVE_BUILD && DEBUG
// local native build => loguru debugging enabled
#define LOGURU_DEBUG_LOGGING 1
#define LOGURU_DEBUG_CHECKS 1
#include <stdexcept>
#include <string>
#else
// jbox build => loguru debugging disabled
#define LOGURU_DEBUG_LOGGING 0
#define LOGURU_DEBUG_CHECKS 0
#endif // LOCAL_NATIVE_BUILD

#include "loguru.hpp"

#include <JukeboxTypes.h>

/**
 * Implementation details
 */
namespace impl {
template<typename... Args>
inline void JBox_LogValues(const char iFile[], TJBox_Int32 iLine, char const *iMessage, Args&& ...iValues)
{
  TJBox_Value values[sizeof...(iValues)] { iValues... };
  JBox_TraceValues(iFile, iLine, iMessage, values, sizeof...(iValues));
}
}

#if DEBUG
/**
 * Allow to write simpler code:
 * ```
 *    // using JBOX_TRACEVALUES
 * 		TJBox_Value instanceIDValue = JBox_MakeNumber(JBox_GetNumber(iParams[0]));
 *		TJBox_Value array[1];
 *		array[0] = instanceIDValue;
 *		JBOX_TRACEVALUES("instance ID = ^0", array, 1);
 *
 *    // using JBOX_LOGVALUES
 *		JBOX_LOGVALUES("instance ID = ^0", iParams[0]));
 * ```
 */
#define JBOX_LOGVALUES(iMessage, ...) \
	::impl::JBox_LogValues(__FILE__, __LINE__, iMessage, __VA_ARGS__)
#else
#define JBOX_LOGVALUES(iMessage, ...)
#endif

#if LOGURU_DEBUG_CHECKS
namespace loguru {
/**
 * This function can be called when the device is created to make loguru output nicer (essentially replaces
 * the name of the thread which is useless, by the name of the rack extension which can be useful when you
 * have different REs using loguru) */
inline void init_for_re(char const *iREName = nullptr)
{
  loguru::g_preamble_thread = false;
  loguru::g_preamble_prefix = iREName;
}

/**
 * This function can be used from tests to replace loguru aborts with exception (which can be checked) */
inline void init_for_test(char const *iPrefix = nullptr)
{
  loguru::g_preamble_thread = false;
  loguru::g_preamble_prefix = iPrefix;
  loguru::set_fatal_handler([](const loguru::Message& message) {
    throw std::runtime_error(std::string(message.prefix) + message.message);
  });
}
}
#endif

#endif