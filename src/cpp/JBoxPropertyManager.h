//
// Created by Yan Pujante on 3/15/15.
//

#ifndef __PongasoftCommon_JBoxPropertyManager_h__
#define __PongasoftCommon_JBoxPropertyManager_h__


#include <map>
#include <Jukebox.h>
#include <vector>

class IJBoxPropertyObserver;
class JBoxNoteStates;
class JBoxNoteListener;

class IJBoxPropertyManager
{
public:
  virtual void registerNoteStates(JBoxNoteStates &iNoteStates) = 0;
  virtual void registerForUpdate(IJBoxPropertyObserver &iJBoxProperty, TJBox_Tag iTag) = 0;
  virtual void registerForInit(IJBoxPropertyObserver &iJBoxProperty) = 0;
  virtual void initProperties() = 0;
};

class JBoxPropertyManager : public IJBoxPropertyManager
{
public:
  JBoxPropertyManager();

  bool onUpdate(TJBox_PropertyDiff const iPropertyDiffs[], TJBox_UInt32 iDiffCount);
  bool onNotesUpdate(TJBox_PropertyDiff const iPropertyDiffs[], TJBox_UInt32 iDiffCount, JBoxNoteListener *iListener);

  virtual void registerNoteStates(JBoxNoteStates &iNoteStates) override;
  virtual void registerForUpdate(IJBoxPropertyObserver &iJBoxProperty, TJBox_Tag iTag) override;
  virtual void registerForInit(IJBoxPropertyObserver &iJBoxProperty) override;
  virtual void initProperties() override;

private:
  struct JBoxPropertyKey
  {
    JBoxPropertyKey(TJBox_ObjectRef iObjectRef, TJBox_Tag iTag): fObjectRef(iObjectRef), fTag(iTag) {}

    TJBox_ObjectRef fObjectRef;
    TJBox_Tag fTag;
  };

  typedef std::map<JBoxPropertyKey, IJBoxPropertyObserver *, bool(*)(JBoxPropertyKey const &, JBoxPropertyKey const &)> JBoxPropertyMap;
  typedef std::vector<IJBoxPropertyObserver *> JBoxPropertyList;

  static bool compare(JBoxPropertyKey const &l, JBoxPropertyKey const &r)
  {
    if(l.fObjectRef == r.fObjectRef)
      return l.fTag < r.fTag;
    else
      return l.fObjectRef < r.fObjectRef;
  }

  JBoxPropertyMap fPropertiesForUpdate;
  JBoxPropertyList fPropertiesForInit;
  JBoxNoteStates *fNoteStates;
};

#endif //__PongasoftCommon_JBoxPropertyManager_h__

