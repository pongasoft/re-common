//
//  JBoxProperty.cpp
//
//  Created by Yan Pujante on 5/22/15.
//
//

#include <Jukebox.h>
#include <logging.h>
#include "JBoxProperty.h"
#include "jbox.h"

//------------------------------------------------------------------------
// JBoxObject::JBoxObject
//------------------------------------------------------------------------
JBoxObject::JBoxObject(char const *iObjectPath):
  fObjectRef(JBox_GetMotherboardObjectRef(iObjectPath))
{
  DCHECK_F(iObjectPath != nullptr);
  DCHECK_F(sizeof(fObjectPath) > strlen(iObjectPath));

#if DEBUG
  strcpy(fObjectPath, iObjectPath);
#endif
}

//------------------------------------------------------------------------
// JBoxPropertyObserver::JBoxPropertyObserver
//------------------------------------------------------------------------
JBoxPropertyObserver::JBoxPropertyObserver(JBoxObject const &parentObject, char const *iPropertyName):
  fPropertyRef(JBox_MakePropertyRef(parentObject.fObjectRef, iPropertyName))
{
  DCHECK_F(iPropertyName != nullptr);
  DCHECK_F(strlen(parentObject.fObjectPath) + 1 /* / */ + strlen(iPropertyName) + 1 /* terminating 0 */ <= sizeof(fPropertyPath));

#if DEBUG
  fmt::printf(std::begin(fPropertyPath), std::end(fPropertyPath), "%s/%s", parentObject.fObjectPath, iPropertyName);
#endif
}

//------------------------------------------------------------------------
// JBoxPropertyObserver::JBoxPropertyObserver
//------------------------------------------------------------------------
JBoxPropertyObserver::JBoxPropertyObserver(char const *iPropertyPath):
  fPropertyRef(jbox::get_property_ref(iPropertyPath))
{
  DCHECK_F(iPropertyPath != nullptr);
  DCHECK_F(sizeof(fPropertyPath) > strlen(iPropertyPath));

#if DEBUG
  strcpy(fPropertyPath, iPropertyPath);
#endif
}
