//
//  JBoxProperty.cpp
//
//  Created by Yan Pujante on 5/22/15.
//
//

#include <Jukebox.h>
#include "JBoxProperty.h"

////////////////////////////////
// JBoxObject
////////////////////////////////
JBoxObject::JBoxObject(char const *iObjectPath):
  fObjectRef(JBox_GetMotherboardObjectRef(iObjectPath))
{
  JBOX_ASSERT(iObjectPath != nullptr);
  JBOX_ASSERT(sizeof(fObjectPath) > strlen(iObjectPath));

  strcpy(fObjectPath, iObjectPath);
}

////////////////////////////////
// JBoxPropertyObserver
////////////////////////////////
JBoxPropertyObserver::JBoxPropertyObserver(JBoxObject const &parentObject, char const *iPropertyName):
  fPropertyRef(JBox_MakePropertyRef(parentObject.fObjectRef, iPropertyName))
{
  JBOX_ASSERT(iPropertyName != nullptr);
  JBOX_ASSERT(strlen(parentObject.fObjectPath) + 1 /* / */ + strlen(iPropertyName) + 1 /* terminating 0 */ <= sizeof(fPropertyPath));

  strcpy(fPropertyPath, parentObject.fObjectPath);
  strcat(fPropertyPath, "/");
  strcat(fPropertyPath, iPropertyName);

}

TJBox_PropertyRef makePropertyRef(char const *iPropertyPath);

/**
 * From a property path ("/cv_outputs/cv_out_1/connected") create a property ref which is made of
 * an object ("/cv_outputs/cv_out_1") and a property name ("connected")
 * @param iPropertyPath
 * @return
 */
TJBox_PropertyRef makePropertyRef(char const *iPropertyPath)
{
  JBOX_ASSERT(iPropertyPath != nullptr);

  char TMP_BUF[kMaxPropertyPathLen + 1];

  JBOX_ASSERT(strlen(iPropertyPath) <= sizeof(TMP_BUF));

  // look for last slash
  char const *lastSlash = strrchr(iPropertyPath, '/');
  // make sure there is a last slash
  JBOX_ASSERT(lastSlash != nullptr);

  // make sure there is enough room (should be since the entire property fits...)
  JBOX_ASSERT(sizeof(TMP_BUF) >= lastSlash - iPropertyPath);

  // copy the object name part ("/cv_outputs/cv_out_1")
  strncpy(TMP_BUF, iPropertyPath, lastSlash - iPropertyPath);
  TMP_BUF[lastSlash - iPropertyPath] = 0;

  // get the object id from the MOM
  TJBox_ObjectRef parentObjectRef = JBox_GetMotherboardObjectRef(TMP_BUF);

  // now lastSlash + 1 points to the property name
  JBOX_ASSERT(sizeof(TMP_BUF) >= strlen(lastSlash + 1));
  strcpy(TMP_BUF, lastSlash + 1); // skipping "/"

  return JBox_MakePropertyRef(parentObjectRef, TMP_BUF);
}

JBoxPropertyObserver::JBoxPropertyObserver(char const *iPropertyPath):
  fPropertyRef(makePropertyRef(iPropertyPath))
{
  JBOX_ASSERT(iPropertyPath != nullptr);
  JBOX_ASSERT(sizeof(fPropertyPath) > strlen(iPropertyPath));

  strcpy(fPropertyPath, iPropertyPath);
}
