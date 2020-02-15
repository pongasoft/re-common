//
// Created by Yan Pujante on 3/15/15.
//

#include <Jukebox.h>
#include "JBoxProperty.h"
#include "JBoxPropertyManager.h"

#ifndef  __phdsp__

#include <iostream>

TJBox_Tag const IGNORED_PROPERTY_TAG =  kJBox_CVInputValue;

#endif // !__phdsp__

JBoxPropertyManager::JBoxPropertyManager() :
  fPropertiesForUpdate(compare), fNoteStates(nullptr)
{
}

bool JBoxPropertyManager::onUpdate(TJBox_PropertyDiff const iPropertyDiffs[], TJBox_UInt32 iDiffCount)
{
  bool stateChanged = false;

  if(iDiffCount != 0)
  {
    JBoxPropertyMap::const_iterator vNotFound = fPropertiesForUpdate.cend();

    // handle regular updates first
    for(TJBox_UInt32 i = 0; i < iDiffCount; i++)
    {
      TJBox_PropertyDiff iPropertyDiff = iPropertyDiffs[i];

      // skipping notes
      if(fNoteStates != nullptr && fNoteStates->isSameObject(iPropertyDiff.fPropertyRef))
      {
        continue;
      }

      JBoxPropertyKey key(iPropertyDiff.fPropertyRef.fObject, iPropertyDiff.fPropertyTag);
      JBoxPropertyMap::const_iterator iter =
        fPropertiesForUpdate.find(key);

      if(iter != vNotFound)
      {
        stateChanged |= iter->second->update(iPropertyDiff);
      }

#ifndef  __phdsp__
      if(iPropertyDiff.fPropertyTag != IGNORED_PROPERTY_TAG)
      {
        TJBox_Value values[] = {
          JBox_MakeNumber(iPropertyDiff.fPropertyTag),
          iPropertyDiff.fPreviousValue,
          iPropertyDiff.fCurrentValue
        };

        if(iter != vNotFound)
        {
          JBOX_TRACEVALUES((std::string("onUpdate: ") + iter->second->getPropertyPath() + "@^0 : ^1 -> ^2").c_str(), values, 3);
        }
        else
        {
          JBOX_TRACEVALUES("onUpdate: /notFound@^0 : ^1 -> ^2", values, 3);
        }
      }
#endif // !__phdsp__

//      TJBox_Value values[] = {
//        JBox_MakeBoolean(iter != vNotFound),
//        JBox_MakeNumber(iPropertyDiff.fPropertyRef.fObject),
//        iPropertyDiff.fPreviousValue,
//        iPropertyDiff.fCurrentValue
//      };
//
//      JBOX_TRACEVALUES("JBoxPropertyObserver::onUpdate @^1 : ^2 -> ^3 (^0)", values, 4);
//      JBOX_TRACE(iPropertyDiff.fPropertyRef.fKey);

    }
  }

  return stateChanged;
}

bool JBoxPropertyManager::onNotesUpdate(const TJBox_PropertyDiff *iPropertyDiffs,
                                        TJBox_UInt32 iDiffCount,
                                        JBoxNoteListener *iListener)
{
  JBOX_ASSERT(fNoteStates != nullptr);
  JBOX_ASSERT(iListener != nullptr);

  bool stateChanged = false;

  if(iDiffCount != 0)
  {
    for(TJBox_UInt32 i = 0; i < iDiffCount; i++)
    {
      TJBox_PropertyDiff iPropertyDiff = iPropertyDiffs[i];

      if(fNoteStates->isSameObject(iPropertyDiff.fPropertyRef))
      {
        stateChanged |= iListener->onNoteReceived(iPropertyDiff);
      }
    }
  }

  return stateChanged;
}


void JBoxPropertyManager::registerForUpdate(IJBoxPropertyObserver &iJBoxProperty, TJBox_Tag iTag)
{
  JBoxPropertyKey key(iJBoxProperty.getPropertyRef().fObject, iTag);
  JBOX_ASSERT_MESSAGE(fPropertiesForUpdate.find(key) == fPropertiesForUpdate.cend(), iJBoxProperty.getPropertyRef().fKey);
  fPropertiesForUpdate[key] = &iJBoxProperty;
#ifndef  __phdsp__
  JBOX_TRACE((std::string("reg4Update: ") + iJBoxProperty.getPropertyPath() + "@" + std::to_string(iJBoxProperty.getPropertyRef().fObject) + "/" + std::to_string(iTag)).c_str());
#endif // !__phdsp__

}

void JBoxPropertyManager::registerForInit(IJBoxPropertyObserver &iJBoxProperty)
{
  fPropertiesForInit.push_back(&iJBoxProperty);
#ifndef  __phdsp__
  JBOX_TRACE((std::string("reg4Init: ") + iJBoxProperty.getPropertyPath() + "@" + std::to_string(iJBoxProperty.getPropertyRef().fObject)).c_str());
#endif // !__phdsp__
}

void JBoxPropertyManager::initProperties()
{
  JBOX_TRACE("JBoxPropertyManager::initProperties()");

  for(auto &&property : fPropertiesForInit)
  {
    property->init();
  }
}

void JBoxPropertyManager::registerNoteStates(JBoxNoteStates &iNoteStates)
{
#ifndef  __phdsp__
  JBOX_TRACE((std::string("registerNoteStates: ") + iNoteStates.fObjectPath + "@" + std::to_string(iNoteStates.fObjectRef)).c_str());
#endif // !__phdsp__
  fNoteStates = &iNoteStates;
}


