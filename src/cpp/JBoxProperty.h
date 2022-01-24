//
//  JBoxProperty.h
//
//  Created by Yan Pujante on 5/22/15.
//
//

#ifndef __PongasoftCommon_JBoxProperty_h__
#define __PongasoftCommon_JBoxProperty_h__

#include "StaticString.h"
#include "jbox.h"
#include <Jukebox.h>
#include "Constants.h"
#include "JBoxPropertyManager.h"
#include "JukeboxTypes.h"
#include <logging.h>

#if DEBUG
namespace Dev
{
  enum EJBoxPropertyState {
    kUninitialized = 0,
    kInSyncWithMOM,
    kInSyncWithCopy,
  };

}
#endif

/**
* Encapsulates an object (with full path).
*/
class JBoxObject
{
public:
  explicit JBoxObject(char const * iObjectPath);

  inline JBoxObject &operator=(JBoxObject const &rhs)
  {
    DCHECK_F(rhs.fObjectRef == fObjectRef, "mismatch object!");
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

#if DEBUG
  TJBox_ObjectName fObjectPath;
#endif
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
#if DEBUG
  virtual char const *getPropertyPath() const = 0;
#endif
  virtual TJBox_PropertyRef const &getPropertyRef() const = 0;

  inline void registerForUpdate(IJBoxPropertyManager &manager, TJBox_Tag iTag) { manager.registerForUpdate(*this, iTag); }
  inline void registerForInit(IJBoxPropertyManager &manager) { manager.registerForInit(*this); }

#if DEBUG
public:
  Dev::EJBoxPropertyState fPropertyState = Dev::kUninitialized;
#endif
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
#if DEBUG
  virtual char const *getPropertyPath() const { return fPropertyPath; };
#endif
  virtual TJBox_PropertyRef const &getPropertyRef() const {return fPropertyRef; };

public:
  TJBox_PropertyRef const fPropertyRef;

private:
#if DEBUG
  char fPropertyPath[kMaxPropertyPathLen + 1];
#endif
};

template<typename T>
class JBoxPropertyUpdateListener
{
public:
  virtual void onPropertyUpdated(T const &previousValue, T const &newValue) = 0;
};

namespace JBox
{
// defaultFromJBoxValue<T>
template <typename T>
inline void defaultFromJBoxValue(TJBox_Value iValue, T &oValue) { oValue = static_cast<T>(JBox_GetNumber(iValue)); }

// defaultFromJBoxValue<TJBox_Bool> (specialization)
template <>
inline void defaultFromJBoxValue(TJBox_Value iValue, TJBox_Bool &oValue) { oValue = JBox_GetBoolean(iValue); }

// defaultFromJBoxValue<bool> (specialization)
template <>
inline void defaultFromJBoxValue(TJBox_Value iValue, bool &oValue) { oValue = static_cast<bool>(JBox_GetBoolean(iValue)); }

// defaultToJBoxValue<T>
template <typename T>
inline TJBox_Value defaultToJBoxValue(T iValue) { return JBox_MakeNumber(static_cast<T>(iValue)); }

// defaultToJBoxValue<TJBox_Bool> (specialization)
template<>
inline TJBox_Value defaultToJBoxValue(TJBox_Bool iValue) { return JBox_MakeBoolean(iValue); }

// defaultToJBoxValue<TJBox_Bool> (bool)
template<>
inline TJBox_Value defaultToJBoxValue(bool iValue) { return JBox_MakeBoolean(iValue); }

// TJBox_Bool
inline TJBox_Bool toJBoxBool(TJBox_Value value) { return JBox_GetBoolean(value); }
inline void toJBoxBool(TJBox_Value value, TJBox_Bool &oValue) { oValue = toJBoxBool(value); }
inline TJBox_Value fromJBoxBool(TJBox_Bool value) { return JBox_MakeBoolean(value); }

// TJBox_Float64
inline TJBox_Float64 toJBoxFloat64(TJBox_Value value) { return JBox_GetNumber(value); }
inline void toJBoxFloat64(TJBox_Value value, TJBox_Float64 &oValue) { oValue = JBox_GetNumber(value); }
inline TJBox_Value fromJBoxFloat64(TJBox_Float64 value) { return JBox_MakeNumber(value); }

// TJBox_Float32
inline TJBox_Float32 toJBoxFloat32(TJBox_Value value) { return static_cast<TJBox_Float32>(JBox_GetNumber(value)); }
inline void toJBoxFloat32(TJBox_Value value, TJBox_Float32 &oValue) { oValue = static_cast<TJBox_Float32>(JBox_GetNumber(value)); }
inline TJBox_Value fromJBoxFloat32(TJBox_Float32 value) { return JBox_MakeNumber(value); }

// TJBox_Int32
inline TJBox_Int32 toJBoxInt32(TJBox_Value value) { return static_cast<TJBox_Int32>(JBox_GetNumber(value)); }
inline void toJBoxInt32(TJBox_Value value, TJBox_Int32 &oValue) { oValue = static_cast<TJBox_Int32>(JBox_GetNumber(value)); }
inline TJBox_Value fromJBoxInt32(TJBox_Int32 value) { return JBox_MakeNumber(value); }

// Enum type
template <typename E>
inline E toEnum(TJBox_Value value) { return static_cast<E>((int) JBox_GetNumber(value)); }
template <typename E>
inline void toEnum(TJBox_Value value, E &oValue) { oValue = static_cast<E>((int) JBox_GetNumber(value)); }
template <typename E>
inline TJBox_Value fromEnum(E value) { return JBox_MakeNumber(value); }

}

/*
 * Actual implementation based on jukebox type
 */
template<typename T, void (* FromJBoxValue)(TJBox_Value, T&) = JBox::defaultFromJBoxValue<T>, TJBox_Value (*ToJBoxValue)(T) = JBox::defaultToJBoxValue<T>>
class JBoxProperty : public JBoxPropertyObserver
{
  static_assert(FromJBoxValue != nullptr || ToJBoxValue != nullptr, "FromJBoxValue and ToJBoxValue cannot both be nullptr");

public:
  typedef T value_type;
  typedef JBoxProperty<T, FromJBoxValue, ToJBoxValue> class_type;

public:
  JBoxProperty(JBoxObject const &parentObject, char const *iPropertyName, T iInitialValue = {}):
   JBoxPropertyObserver(parentObject, iPropertyName), fValue{iInitialValue}
  {
  }

  explicit JBoxProperty(char const *iPropertyPath, T iInitialValue = {}): JBoxPropertyObserver(iPropertyPath), fValue{iInitialValue} {}

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

#if DEBUG
    JBOX_ASSERT(fPropertyState != Dev::kInSyncWithMOM);
    JBOX_ASSERT(other.fPropertyState == Dev::kInSyncWithMOM);
    fPropertyState = Dev::kInSyncWithCopy;
#endif

    fValue = other.getValue();

    return *this;
  }

  /**
   * Called by the manager for properties registered for receiving updates
   */
  virtual bool update(const TJBox_PropertyDiff &iPropertyDiff)
  {
    if constexpr (FromJBoxValue != nullptr)
    {
      JBOX_ASSERT_MESSAGE(JBox_IsReferencingSameProperty(iPropertyDiff.fPropertyRef, fPropertyRef),
                          "mismatch object!");
      T prev = fValue;
      setJBoxValue(iPropertyDiff.fCurrentValue);

#if DEBUG
      DCHECK_F(fPropertyState != Dev::kInSyncWithCopy);
      fPropertyState = Dev::kInSyncWithMOM;
#endif

      if(fUpdateListener != nullptr)
        fUpdateListener->onPropertyUpdated(prev, fValue);

      return prev != fValue;
    }
    else
    {
      JBOX_ASSERT_MESSAGE(false, "Write only property. Should not be called.");
      return false;
    }
  }

  /**
   * Initializes the property to make sure it is in sync with the motherboard
   */
  virtual void init()
  {
#if DEBUG
    DCHECK_F(fPropertyState == Dev::kUninitialized, "FAILURE: init() -> property initialized multiple times %s", getPropertyPath());
#endif
    if constexpr (FromJBoxValue != nullptr)
      // property can be read => read it from the motherboard
      loadValueFromMotherboard();
    else
      // property cannot be read => initializes the motherboard with initial value
      initMotherboard(fValue);
  }

  /**
   * Initializes the property and propagate to motherboard
   */
  void initMotherboard(T iValue)
  {
#if DEBUG
    DCHECK_F(fPropertyState == Dev::kUninitialized, "FAILURE: initMotherboard() -> property initialized multiple times %s", getPropertyPath());
    fPropertyState = Dev::kInSyncWithMOM;
#endif
    doSetValue(iValue);
    storeValueToMotherboard();
  }

  /**
   * Accesses the value (in dev mode, make sure it is initialized!)
   */
  inline T getValue() const {

#if DEBUG
    DCHECK_F(fPropertyState != Dev::kUninitialized, "FAILURE: getValue() -> Accessing uninitialized property %s", getPropertyPath());
#endif

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
   * Checks this value against `ioPreviousValue` and update `ioPreviousValue` with the new value if changed
   * @return `true` if the value has changed, `false` otherwise */
  inline bool updatePreviousValueOnChange(T &ioPreviousValue) const
  {
    return updatePreviousValueOnChange(getValue(), ioPreviousValue);
  }

  /**
   * Checks `iNewValue` against `ioPreviousValue` and update `ioPreviousValue` with the new value if changed
   * @return `true` if the value has changed, `false` otherwise */
  static constexpr bool updatePreviousValueOnChange(T const &iNewValue, T &ioPreviousValue)
  {
    if(iNewValue != ioPreviousValue)
    {
      ioPreviousValue = iNewValue;
      return true;
    }

    return false;
  }

  /**
   * Conditionally stores the value passed in this property and propagate the motherboard if different only
   */
  bool storeValueToMotherboardOnUpdate(T iValue)
  {
#if DEBUG
    DCHECK_F(fPropertyState == Dev::kInSyncWithMOM, "FAILURE: storeValueToMotherboardOnUpdate() -> should be in sync with MOM %s", getPropertyPath());
#endif

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
//   * Unconditionally stores the value passed in this property and propagate the motherboard.
//   */
//  void storeValueToMotherboard(T iValue)
//  {
//#if DEBUG
//    fPropertyState = Dev::kInSyncWithMOM;
//#endif
//    doSetValue(iValue);
//    storeValueToMotherboard();
//  }

private:
  /**
   * Loads the raw value from the MOM. Note that this method does NOT modify this object.
   * Use `loadValue` / `storeValueToMotherboardOnUpdate` in general */
  inline TJBox_Value loadRawValue() const { return JBox_LoadMOMProperty(fPropertyRef); }

  /**
   * Stores the raw value to the MOM. Note that this method does NOT modify this object */
  inline void storeRawValue(TJBox_Value const &iValue) const { JBox_StoreMOMProperty(fPropertyRef, iValue); }

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
    setJBoxValue(loadRawValue());
#if DEBUG
    fPropertyState = Dev::kInSyncWithMOM;
#endif
    return getValue();
  }

  /**
   * Store the current value held by this property into the motherboard
   */
  inline void storeValueToMotherboard()
  {
#if DEBUG
    DCHECK_F(fPropertyState != Dev::kUninitialized, "FAILURE: storeValueToMotherboard() -> Accessing uninitialized property %s", getPropertyPath());
#endif

    storeRawValue(getJBoxValue());

#if DEBUG
    fPropertyState = Dev::kInSyncWithMOM;
#endif
  }


private:
  T fValue;
  JBoxPropertyUpdateListener<T> *fUpdateListener{};

  inline void setJBoxValue(TJBox_Value value)
  {
    static_assert(FromJBoxValue != nullptr, "Write Only Property. Should not be called!");
    FromJBoxValue(value, fValue);
  }

  inline TJBox_Value getJBoxValue() {
    static_assert(ToJBoxValue != nullptr, "Read Only Property. Should not be called!");
    return ToJBoxValue(fValue);
  }
};


typedef JBoxProperty<TJBox_Bool> BooleanJBoxProperty;
typedef JBoxProperty<TJBox_Float64> Float64JBoxProperty;
typedef JBoxProperty<TJBox_Float32> Float32JBoxProperty;
typedef JBoxProperty<TJBox_Int32> Int32JBoxProperty;

/**
 * Shortcut definition when the property is read only. */
template<typename T, void (* FromJBoxValue)(TJBox_Value, T&) = JBox::defaultFromJBoxValue<T>>
using ReadOnlyJBoxProperty = JBoxProperty<T, FromJBoxValue, nullptr>;

/**
 * Shortcut definition when the property is write only. */
template<typename T, TJBox_Value (*ToJBoxValue)(T) = JBox::defaultToJBoxValue<T>>
using WriteOnlyJBoxProperty = JBoxProperty<T, nullptr, ToJBoxValue>;

// BuiltIn OnOffByPass for effects
class BuiltInOnOffBypassJBoxProperty : public JBoxProperty<TJBox_OnOffBypassStates>
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
