//
// Created by Yan Pujante on 3/15/15.
//

#include <Jukebox.h>
#include "JBoxProperty.h"
#include "JBoxPropertyManager.h"
#include <logging.h>

#if LOCAL_NATIVE_BUILD && RE_COMMON_JBoxPropertyManager_ENABLE_LOGGING
// Can only include <string> in native build
#include <string>
constexpr TJBox_Tag IGNORED_PROPERTY_TAG =  kJBox_CVInputValue;
#endif // RE_COMMON_JBoxPropertyManager_ENABLE_LOGGING

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

#if LOCAL_NATIVE_BUILD && RE_COMMON_JBoxPropertyManager_ENABLE_LOGGING && DEBUG
      if(iPropertyDiff.fPropertyTag != IGNORED_PROPERTY_TAG)
      {
        if(iter != vNotFound)
        {
          JBOX_LOGVALUES((std::string("onUpdate: ") + iter->second->getPropertyPath() + "@^0 : ^1 -> ^2").c_str(), JBox_MakeNumber(iPropertyDiff.fPropertyTag), iPropertyDiff.fPreviousValue, iPropertyDiff.fCurrentValue);
        }
        else
        {
          JBOX_LOGVALUES("onUpdate: /notFound@^0 : ^1 -> ^2", JBox_MakeNumber(iPropertyDiff.fPropertyTag), iPropertyDiff.fPreviousValue, iPropertyDiff.fCurrentValue);
        }
      }
#endif

//#if RE_COMMON_JBoxPropertyManager_ENABLE_LOGGING && DEBUG
//      JBOX_LOGVALUES("JBoxPropertyObserver::onUpdate @^1 : ^2 -> ^3 (^0)",
//                     JBox_MakeBoolean(iter != vNotFound),
//                     JBox_MakeNumber(iPropertyDiff.fPropertyRef.fObject),
//                     iPropertyDiff.fPreviousValue,
//                     iPropertyDiff.fCurrentValue);
//      JBOX_TRACE(iPropertyDiff.fPropertyRef.fKey);
//#endif

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

#if RE_COMMON_JBoxPropertyManager_ENABLE_LOGGING
  DLOG_F(INFO, "reg4Update: %s@%d/%d", iJBoxProperty.getPropertyPath(), iJBoxProperty.getPropertyRef().fObject, iTag);
#endif
}

void JBoxPropertyManager::registerForInit(IJBoxPropertyObserver &iJBoxProperty)
{
  fPropertiesForInit.push_back(&iJBoxProperty);

#if RE_COMMON_JBoxPropertyManager_ENABLE_LOGGING
  DLOG_F(INFO, "reg4Init: %s@%d", iJBoxProperty.getPropertyPath(), iJBoxProperty.getPropertyRef().fObject);
#endif
}

void JBoxPropertyManager::initProperties()
{
#if RE_COMMON_JBoxPropertyManager_ENABLE_LOGGING
  DLOG_F(INFO, "JBoxPropertyManager::initProperties()");
#endif

  for(auto &&property : fPropertiesForInit)
  {
    property->init();
  }
}

void JBoxPropertyManager::registerNoteStates(JBoxNoteStates &iNoteStates)
{
#if RE_COMMON_JBoxPropertyManager_ENABLE_LOGGING
  DLOG_F(INFO, "registerNoteStates: %s@%d", iNoteStates.fObjectPath, iNoteStates.fObjectRef);
#endif

  fNoteStates = &iNoteStates;
}


