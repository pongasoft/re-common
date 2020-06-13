//
// Created by Yan Pujante on 3/15/15.
//

#include <Jukebox.h>
#include "JBoxProperty.h"
#include "JBoxPropertyManager.h"
#include <logging/logging.h>

#ifdef LOCAL_NATIVE_BUILD

#include <iostream>
constexpr TJBox_Tag IGNORED_PROPERTY_TAG =  kJBox_CVInputValue;

#endif // LOCAL_NATIVE_BUILD

JBoxPropertyManager::JBoxPropertyManager() :
  fPropertiesForUpdate(compare), fNoteStates(nullptr)
{
}

bool JBoxPropertyManager::onUpdate(TJBox_PropertyDiff const iPropertyDiffs[], TJBox_UInt32 iDiffCount)
{
  bool stateChanged = false;

  if(iDiffCount != 0)
  {
    auto vNotFound = fPropertiesForUpdate.cend();

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
      auto iter = fPropertiesForUpdate.find(key);

      if(iter != vNotFound)
      {
        stateChanged |= iter->second->update(iPropertyDiff);
      }

#ifdef LOCAL_NATIVE_BUILD
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
#endif

      #ifndef  __phdsp__
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
  DCHECK_F(fNoteStates != nullptr);
  DCHECK_F(iListener != nullptr);

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
  DCHECK_F(fPropertiesForUpdate.find(key) == fPropertiesForUpdate.cend(), "Key not found [%s]", iJBoxProperty.getPropertyRef().fKey);
  fPropertiesForUpdate[key] = &iJBoxProperty;
  DLOG_F(INFO, "reg4Update: %s@%d/%d", iJBoxProperty.getPropertyPath(), iJBoxProperty.getPropertyRef().fObject, iTag);
}

void JBoxPropertyManager::registerForInit(IJBoxPropertyObserver &iJBoxProperty)
{
  fPropertiesForInit.push_back(&iJBoxProperty);
  DLOG_F(INFO, "reg4Init: %s@%d", iJBoxProperty.getPropertyPath(), iJBoxProperty.getPropertyRef().fObject);
}

void JBoxPropertyManager::initProperties()
{
  DLOG_F(INFO, "JBoxPropertyManager::initProperties()");

  for(auto &&property : fPropertiesForInit)
  {
    property->init();
  }
}

void JBoxPropertyManager::registerNoteStates(JBoxNoteStates &iNoteStates)
{
  DLOG_F(INFO, "registerNoteStates: %s@%d", iNoteStates.fObjectPath, iNoteStates.fObjectRef);
  fNoteStates = &iNoteStates;
}


