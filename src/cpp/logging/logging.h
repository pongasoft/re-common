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
#else
// jbox build => loguru debugging disabled
#define LOGURU_DEBUG_LOGGING 0
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
	impl::JBox_LogValues(__FILE__, __LINE__, iMessage, __VA_ARGS__)
#else
#define JBOX_LOGVALUES(iMessage, ...)
#endif

#endif