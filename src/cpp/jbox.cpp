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

#include "jbox.h"
#include "logging/logging.h"

namespace jbox {

//------------------------------------------------------------------------
// get_property_ref
//------------------------------------------------------------------------
TJBox_PropertyRef get_property_ref(char const *iPropertyPath) noexcept
{
  DCHECK_F(iPropertyPath != nullptr);

  char buf[kMaxPropertyPathLen + 1];

  // make sure the property fits
  DCHECK_F(strlen(iPropertyPath) <= sizeof(buf));

  // look for last slash
  char const *lastSlash = strrchr(iPropertyPath, '/');

  // make sure there is a last slash
  DCHECK_F(lastSlash != nullptr);
  // make sure there is enough room (should be since the entire property fits...)
  DCHECK_F(sizeof(buf) >= lastSlash - iPropertyPath);

  // copy the object name part ("/cv_outputs/cv_out_1")
  strncpy(buf, iPropertyPath, lastSlash - iPropertyPath);
  buf[lastSlash - iPropertyPath] = 0;

  // get the object id from the MOM
  TJBox_ObjectRef parentObjectRef = JBox_GetMotherboardObjectRef(buf);

  // now lastSlash + 1 points to the property name
  DCHECK_F(sizeof(buf) >= strlen(lastSlash + 1));
  strcpy(buf, lastSlash + 1); // skipping "/"

  return JBox_MakePropertyRef(parentObjectRef, buf);
}

}