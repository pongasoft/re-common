//
//  JBoxProperty.h
//
//  Created by Yan Pujante on 5/22/15.
//
//

#ifndef __PongasoftCommon_JBoxProperty_h__
#define __PongasoftCommon_JBoxProperty_h__

#include <Jukebox.h>
#include "Constants.h"
#include "JBoxPropertyManager.h"
#include "JukeboxTypes.h"

#ifndef  __phdsp__

#include <iostream>

#endif // !__phdsp__

#ifndef  __phdsp__
namespace Dev
{
  enum EJBoxPropertyState {
    kUninitialized = 0,
    kInSyncWithMOM,
    kInSyncWithCopy,
  };

}
#endif // !__phdsp__

/**
* Encapsulates an object (with full path).
*/
class JBoxObject
{
public:
  JBoxObject(char const * iObjectPath);

  inline JBoxObject &operator=(JBoxObject const &rhs)
  {
    JBOX_ASSERT_MESSAGE(rhs.fObjectRef == fObjectRef, "mismatch object!");
    return *this;
  }

  inline bool isSameObject(TJBox_ObjectRef const iObjectRef) const
  {
    return fObjectRef == iObjectRef;
  }

  inline bool isSameObject(TJBox_PropertyRef const iPropertyRef) const
  {
    return isSameObject(iPropertyRef.fObject);
  }

public:
  TJBox_ObjectRef const fObjectRef;

  TJBox_ObjectName fObjectPath;
};

/*
 * Listener for note
 */
class JBoxNoteListener
{
public:
  virtual bool onNoteReceived(const TJBox_PropertyDiff &iPropertyDiff) = 0;
};

/*
 * Represents the note states object
 */
class JBoxNoteStates : public JBoxObject
{
public:
  JBoxNoteStates() : JBoxObject("/note_states") {}
};

/*
 * Abstract class defines the observer pattern
 */
class IJBoxPropertyObserver
{
public:
  virtual bool update(const TJBox_PropertyDiff &iPropertyDiff) = 0;
  virtual void init() = 0;
  virtual char const *getPropertyPath() const = 0;
  virtual TJBox_PropertyRef const &getPropertyRef() const = 0;

  inline void registerForUpdate(IJBoxPropertyManager &manager, TJBox_Tag iTag) { manager.registerForUpdate(*this, iTag); }
  inline void registerForInit(IJBoxPropertyManager &manager) { manager.registerForInit(*this); }

#ifndef  __phdsp__
public:
  Dev::EJBoxPropertyState fPropertyState = Dev::kUninitialized;
#endif // !__phdsp__
};

/*
 * Base class defines the observer pattern
 */
class JBoxPropertyObserver : public IJBoxPropertyObserver
{
public:
  JBoxPropertyObserver(JBoxObject const &parentObject, char const *iPropertyName);
  JBoxPropertyObserver(char const *iPropertyPath);

  //virtual bool update(const TJBox_PropertyDiff &iPropertyDiff) = 0;
  virtual char const *getPropertyPath() const { return fPropertyPath; };
  virtual TJBox_PropertyRef const &getPropertyRef() const {return fPropertyRef; };

public:
  TJBox_PropertyRef const fPropertyRef;

private:
  char fPropertyPath[kMaxPropertyPathLen + 1];
};

template<typename T>
class JBoxPropertyUpdateListener
{
public:
  virtual void onPropertyUpdated(T const &previousValue, T const &newValue) = 0;
};

/*
 * Actual implementation based on jukebox type
 */
template<typename T, T (* FromJBoxValue)(TJBox_Value), TJBox_Value (*ToJBoxValue)(T)>
class JBoxProperty : public JBoxPropertyObserver
{
public:
  typedef T value_type;
  typedef JBoxProperty<T, FromJBoxValue, ToJBoxValue> class_type;

public:
  JBoxProperty(JBoxObject const &parentObject, char const *iPropertyName):
   JBoxPropertyObserver(parentObject, iPropertyName), fUpdateListener(nullptr)
   {
   }

  JBoxProperty(char const *iPropertyPath):
    JBoxPropertyObserver(iPropertyPath), fUpdateListener(nullptr)
  {
  }

  /**
   * A listener invoked after a property update (only called in update!)
   */
  void setUpdateListener(JBoxPropertyUpdateListener<T> *iUpdateListener)
  {
    fUpdateListener = iUpdateListener;
  }

  /**
   * Called to copy the current property into the previous property
   */
  inline class_type &operator=(const class_type &other)
  {
    JBOX_ASSERT_MESSAGE(JBox_IsReferencingSameProperty(other.fPropertyRef, fPropertyRef), "mismatch object!");

#ifndef  __phdsp__
    JBOX_ASSERT(fPropertyState != Dev::kInSyncWithMOM);
    JBOX_ASSERT(other.fPropertyState == Dev::kInSyncWithMOM);
    fPropertyState = Dev::kInSyncWithCopy;
#endif // !__phdsp__

    fValue = other.getValue();

    return *this;
  }

  /**
   * Called by the mamanager for properties registered for receiving updates
   */
  virtual bool update(const TJBox_PropertyDiff &iPropertyDiff)
  {
    JBOX_ASSERT_MESSAGE(JBox_IsReferencingSameProperty(iPropertyDiff.fPropertyRef, fPropertyRef), 
                        "mismatch object!");
    T prev = fValue;
    setJBoxValue(iPropertyDiff.fCurrentValue);

#ifndef  __phdsp__
    JBOX_ASSERT(fPropertyState != Dev::kInSyncWithCopy);
    fPropertyState = Dev::kInSyncWithMOM;
#endif // !__phdsp__

    if(fUpdateListener != nullptr)
      fUpdateListener->onPropertyUpdated(prev, fValue);

    return prev != fValue;
  }

  /**
   * Initializes the property to make sure it is in sync with the motherboard
   */
  virtual void init()
  {
#ifndef  __phdsp__
    JBOX_ASSERT_MESSAGE(fPropertyState == Dev::kUninitialized, (std::string("FAILURE: init() -> property initialized multiple times ") + getPropertyPath()).c_str());
    fPropertyState = Dev::kInSyncWithMOM;
#endif // !__phdsp__
    loadValueFromMotherboard();
  }

  /**
   * Initializes the property and propagate to motherboard
   */
  void initMotherboard(T iValue)
  {
#ifndef  __phdsp__
    JBOX_ASSERT_MESSAGE(fPropertyState == Dev::kUninitialized, (std::string("FAILURE: initMotherboard() -> property initialized multiple times ") + getPropertyPath()).c_str());
    fPropertyState = Dev::kInSyncWithMOM;
#endif // !__phdsp__
    doSetValue(iValue);
    storeValueToMotherboard();
  }

  /**
   * Accesses the value (in dev mode, make sure it is initialized!)
   */
  inline T getValue() const {

#ifndef  __phdsp__
    JBOX_ASSERT_MESSAGE(fPropertyState != Dev::kUninitialized, (std::string("FAILURE: getValue() -> Accessing uninitialized property ") + getPropertyPath()).c_str());
#endif // !__phdsp__

    return fValue;
  }

  /**
   * Shortcut to compare previous and current properties
   */
  inline bool isSameValue(class_type const &other) const { return getValue() == other.getValue(); }

  /**
 * Shortcut to compare previous and current properties
 */
  inline bool isNotSameValue(class_type const &other) const { return getValue() != other.getValue(); }

  /**
   * Conditionnally stores the value passed in this property and propagate the motherboard if different only
   */
  bool storeValueToMotherboardOnUpdate(T iValue)
  {
#ifndef  __phdsp__
    JBOX_ASSERT_MESSAGE(fPropertyState == Dev::kInSyncWithMOM, (std::string("FAILURE: storeValueToMotherboardOnUpdate() -> should be in sync with MOM ") + getPropertyPath()).c_str());
#endif // !__phdsp__

    auto previousValue = getValue();

    if(previousValue != iValue)
    {
      doSetValue(iValue);
      storeValueToMotherboard();
      return true;
    }
    return false;
  }

//  /**
//   * Unconditionnally stores the value passed in this property and propagate the motherboard (even if current value
//   * is the same)
//   */
//  bool storeValueToMotherboard(T iValue)
//  {
//    auto previousValue = getValue();
//
//    doSetValue(iValue);
//    storeValueToMotherboard();
//
//    return previousValue != iValue;
//  }

private:
  /**
   * Sets the value only (not propagated to motherboard!)
   * Note that it is private because it should only be used by this class: dangerous as it leaves this property not in sync
   */
  inline void doSetValue(T iValue)
  {
    fValue = iValue;
  }

 /**
  * Unconditionnaly loads the value from the MOM
  * Note this method could be public if there is a need for not using the update mechanism, keeping private for now
  */
  inline T loadValueFromMotherboard()
  {
    setJBoxValue(JBox_LoadMOMProperty(fPropertyRef));
#ifndef  __phdsp__
    fPropertyState = Dev::kInSyncWithMOM;
#endif // !__phdsp__
    return getValue();
  }

  /**
   * Store the current value held by this property into the motherboard
   */
  inline void storeValueToMotherboard()
  {
#ifndef  __phdsp__
    JBOX_ASSERT_MESSAGE(fPropertyState != Dev::kUninitialized, (std::string("FAILURE: storeValueToMotherboard() -> Accessing uninitialized property ") + getPropertyPath()).c_str());
#endif // !__phdsp__

    JBox_StoreMOMProperty(fPropertyRef, getJBoxValue());

#ifndef  __phdsp__
    fPropertyState = Dev::kInSyncWithMOM;
#endif // !__phdsp__
  }


private:
  T fValue;
  JBoxPropertyUpdateListener<T> *fUpdateListener;

  inline void setJBoxValue(TJBox_Value value) {
    doSetValue(FromJBoxValue(value));
  }

  inline TJBox_Value getJBoxValue() {
    return ToJBoxValue(fValue);
  }
};

namespace JBox
{
// TJBox_Bool
  inline TJBox_Bool toJBoxBool(TJBox_Value value)
  {
    return JBox_GetBoolean(value);
  }
  inline TJBox_Value fromJBoxBool(TJBox_Bool value)
  {
    return JBox_MakeBoolean(value);
  }

// TJBox_Float64
  inline TJBox_Float64 toJBoxFloat64(TJBox_Value value)
  {
    return JBox_GetNumber(value);
  }
  inline TJBox_Value fromJBoxFloat64(TJBox_Float64 value)
  {
    return JBox_MakeNumber(value);
  }

// TJBox_Float32
  inline TJBox_Float32 toJBoxFloat32(TJBox_Value value)
  {
    return static_cast<TJBox_Float32>(JBox_GetNumber(value));
  }
  inline TJBox_Value fromJBoxFloat32(TJBox_Float32 value)
  {
    return JBox_MakeNumber(value);
  }

  // TJBox_Int32
  inline TJBox_Int32 toJBoxInt32(TJBox_Value value)
  {
    return static_cast<TJBox_Int32>(JBox_GetNumber(value));
  }
  inline TJBox_Value fromJBoxInt32(TJBox_Int32 value)
  {
    return JBox_MakeNumber(value);
  }

  // Enum type
  template <typename E>
  inline E toEnum(TJBox_Value value)
  {
    return static_cast<E>((int) JBox_GetNumber(value));
  }
  template <typename E>
  inline TJBox_Value fromEnum(E value)
  {
    return JBox_MakeNumber(value);
  }

  // For Read Only properties
  template <typename T>
  inline TJBox_Value readOnly(T value)
  {
    (void) value; // unused
    throw "should not be reached";
  }

}

typedef JBoxProperty<TJBox_Bool, JBox::toJBoxBool, JBox::fromJBoxBool> BooleanJBoxProperty;
typedef JBoxProperty<TJBox_Float64, JBox::toJBoxFloat64, JBox::fromJBoxFloat64> Float64JBoxProperty;
typedef JBoxProperty<TJBox_Float32, JBox::toJBoxFloat32, JBox::fromJBoxFloat32> Float32JBoxProperty;
typedef JBoxProperty<TJBox_Int32, JBox::toJBoxInt32, JBox::fromJBoxInt32> Int32JBoxProperty;

// BuiltIn OnOffByPass for effects
class BuiltInOnOffBypassJBoxProperty : public JBoxProperty<TJBox_OnOffBypassStates, JBox::toEnum<TJBox_OnOffBypassStates>, JBox::fromEnum<TJBox_OnOffBypassStates> >
{
public:
  BuiltInOnOffBypassJBoxProperty() : JBoxProperty("/custom_properties/builtin_onoffbypass")
  {
  }

  inline void registerForUpdate(IJBoxPropertyManager &manager)
  {
    manager.registerForUpdate(*this, kJBox_CustomPropertiesOnOffBypass);
  }

  inline bool isOn() const { return getValue() == kJBox_EnabledOn; }
  inline bool isOff() const { return getValue() == kJBox_EnabledOff; }
  inline bool isBypassed() const { return getValue() == kJBox_EnabledBypass; }
};

#endif /* define(__PongasoftCommon_JBoxProperty_h__) */
