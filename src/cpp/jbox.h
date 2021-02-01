/*
 * Copyright (c) 2021 pongasoft
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

#ifndef __PongasoftCommon_jbox_h__
#define __PongasoftCommon_jbox_h__

#include <Jukebox.h>
#include "fmt.h"
#include "StaticString.h"

namespace jbox {

/**
 * max size for a full path property name (ex: `/custom_properties/my_prop`) (not including terminating 0)
 * [objectName] / [propertyName] (/ is included in object name but is not in propertyName)
 */
constexpr size_t kMaxPropertyPathLen = kJBox_MaxObjectNameLen + 1 + kJBox_MaxPropertyNameLen;

/**
 * Defining useful types */
using ObjectPath = StaticString<kJBox_MaxObjectNameLen + 1>;
using PropertyName = StaticString<kJBox_MaxPropertyNameLen + 1>;
using PropertyPath = StaticString<kMaxPropertyPathLen + 1>;

/**
 * Convenient call to create a full path to a custom property given just its name
 */
constexpr PropertyPath custom_property(PropertyName iPropertyName) noexcept {
  return PropertyPath::printf("/custom_properties/%s", iPropertyName);
}

/**
 * Convenient call to create a full path to a custom property given just its name (as a format + arguments) */
template<typename... Args>
constexpr PropertyPath custom_property(char const *iFormat, Args &&... args) noexcept
{
  return custom_property(PropertyName::printf(iFormat, std::forward<Args>(args)...));
}

/**
 * Given the full path to an object in the motherboard definition, returns the object reference.
 *
 * Example:
 *
 * ```
 * auto customProperties = jbox::get_object_ref("/custom_properties");
 * ```
 *
 * @note an object path always start with a `/`
 */
inline TJBox_ObjectRef get_object_ref(char const *iObjectPath) noexcept
{
  return JBox_GetMotherboardObjectRef(iObjectPath);
}

/**
 * Given a format and arguments for the path to an object in the motherboard definition, returns the object reference.
 *
 * Example:
 * ```
 * int N = 4;
 * for(int i = 0; i < N; i++)
 * {
 *   fSampleZoneObjectRefs[i] = jbox::get_object_ref("/user_samples/%d", i);
 * }
 * ```
 *
 * @note an object path always start with a `/`
 *
 * @see `fmt::printf` for details on format
 */
template<typename... Args>
inline TJBox_ObjectRef get_object_ref(char const *iObjectPathFormat, Args &&... args) noexcept
{
  return JBox_GetMotherboardObjectRef(ObjectPath::printf(iObjectPathFormat, std::forward<Args>(args)...));
}

/**
 * Given a previously obtained object reference and the property name (for a property defined in the motherboard),
 * returns the reference to the property.
 *
 * Example:
 * ```
 * fMotherBoardCustomPropertiesRef = jbox::get_object_ref("/custom_properties");
 * fNoteOnPropertyRef = jbox::get_property_ref(fMotherBoardCustomPropertiesRef, "noteon");
 * ```
 *
 * @note the property name does NOT start with `/`
 */
inline TJBox_PropertyRef get_property_ref(TJBox_ObjectRef iObject, char const *iPropertyName) noexcept
{
  return JBox_MakePropertyRef(iObject, iPropertyName);
}

/**
 * Given a previously obtained object reference and a format and arguments for a property name
 * (for a property defined in the motherboard), returns the reference to the property.
 *
 * Example:
 * ```
 * fMotherBoardCustomPropertiesRef = jbox::get_object_ref("/custom_properties");
 * int N = 4;
 * for(int i = 0; i < N; i++)
 * {
 *   fSampleNativeObjectRefs[i] = jbox::get_property_ref(fMotherBoardCustomPropertiesRef,
 *                                                       "sample_sound_native_object%d", i);
 * }
 * ```
 *
 * @note the property name does NOT start with `/`
 *
 * @see `fmt::printf` for details on format
 */
template<typename... Args>
inline TJBox_PropertyRef get_property_ref(TJBox_ObjectRef iObject, char const *iPropertyNameFormat, Args &&... args) noexcept
{
  return JBox_MakePropertyRef(iObject, PropertyName::printf(iPropertyNameFormat, std::forward<Args>(args)...));
}

/**
 * Given the full path to a property defined in the motherboard, returns the reference to the property.
 *
 * Example:
 * ```
 * fNoteOnPropertyRef = jbox::get_property_ref("/custom_properties/noteon");
 * ```
 *
 * @note the full property path start with `/`
 */
TJBox_PropertyRef get_property_ref(char const *iPropertyPath) noexcept;

/**
 * Given a format and arguments to the full path to a property defined in the motherboard,
 * returns the reference to the property.
 *
 * Example:
 * ```
 * int N = 4;
 * for(int i = 0; i < N; i++)
 * {
 *   fSampleNativeObjectRefs[i] = jbox::get_property_ref("/custom_properties/sample_sound_native_object%d", i);
 * }
 * ```
 *
 * @note the full property path start with `/`
 *
 * @see `fmt::printf` for details on format
 */
template<typename... Args>
inline TJBox_PropertyRef get_property_ref(char const *iPropertyPathFormat, Args &&... args) noexcept
{
  return get_property_ref(PropertyPath::printf(iPropertyPathFormat, std::forward<Args>(args)...));
}

}

#endif //__PongasoftCommon_jbox_h__
