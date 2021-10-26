//
// Created by Yan Pujante on 3/14/15.
//

#ifndef __PongasoftCommon_CVSocket_h__
#define __PongasoftCommon_CVSocket_h__

#include "Jukebox.h"
#include "JBoxProperty.h"
#include "JBoxPropertyManager.h"

/**
* TCVSocket
*/
template <typename TJBoxProperty>
class TCVSocket
{
public:
  explicit TCVSocket(char const *iSocketPath):
    iSocketObject(iSocketPath), fPropConnected(iSocketObject, "connected"), fPropValue(iSocketObject, "value") {}

  virtual void registerForUpdate(IJBoxPropertyManager &manager) = 0;

  inline TJBox_Bool isConnected() const { return fPropConnected.getValue(); }

  inline bool isNewlyConnected(TCVSocket<TJBoxProperty> const &previous) const
  {
    return !previous.isConnected() && isConnected();
  }

protected:
  JBoxObject iSocketObject;
  BooleanJBoxProperty fPropConnected;
  TJBoxProperty fPropValue;
};

template <typename T>
class TCVInSocket;

/**
* TCVOutSocket
*/
template <typename TJBoxProperty>
class TCVOutSocket: public TCVSocket<TJBoxProperty>
{
public:
  explicit TCVOutSocket(char const *iSocketName) :
    TCVSocket<TJBoxProperty>(jbox::ObjectPath::printf("/cv_outputs/%s", iSocketName))
  {};

  virtual void registerForUpdate(IJBoxPropertyManager &manager)
  {
    manager.registerForUpdate(this->fPropConnected, kJBox_CVOutputConnected);
    manager.registerForInit(this->fPropValue);
  }

  void initMotherboard(typename TJBoxProperty::value_type iValue)
  {
    this->fPropValue.initMotherboard(iValue);
  }

  bool storeValueToMotherboardOnUpdate(typename TJBoxProperty::value_type value)
  {
    return this->fPropValue.storeValueToMotherboardOnUpdate(value);
  }

  template <typename U>
  friend class TCVInSocket;
};

/**
 * TCVInSocket
 */
template <typename TJBoxProperty>
class TCVInSocket: public TCVSocket<TJBoxProperty>
{
public:
  explicit TCVInSocket(char const *iSocketName) :
    TCVSocket<TJBoxProperty>(jbox::ObjectPath::printf("/cv_inputs/%s", iSocketName))
  {};

  virtual void registerForUpdate(IJBoxPropertyManager &manager) {
    manager.registerForUpdate(this->fPropConnected, kJBox_CVInputConnected);
    manager.registerForUpdate(this->fPropValue, kJBox_CVInputValue);
  };

  inline typename TJBoxProperty::value_type getValue() const { return this->fPropValue.getValue(); }

  inline bool isSameValue(TCVInSocket<TJBoxProperty> const &other) const
  {
    return this->isConnected() == other.isConnected() && getValue() == other.getValue();
  }

  inline bool isNotSameValue(TCVInSocket<TJBoxProperty> const &other) const
  {
    return !isSameValue(other);
  }

  /**
   * Copies value (meaning there is no conversion/loss) from this 'in' socket to the provided
   * 'out' socket only when both sockets are connected. */
  template<typename UJBoxProperty>
  inline void maybeCopyValue(TCVOutSocket<UJBoxProperty> &oSocket)
  {
    if(this->isConnected() && oSocket.isConnected())
      oSocket.fPropValue.storeValueToMotherboardOnUpdate(static_cast<typename UJBoxProperty::value_type>(this->fPropValue.getValue()));
  }
};

namespace JBox {
/*
  inline TJBox_Int32 getAsVelocityValue() const { return clamp<TJBox_Int32>(getValue() * 127.f, 0, 127); }
 */

inline TJBox_Int32 toNote(TJBox_Float64 value)
{
  return clamp<TJBox_Int32>(value * 127.f + 0.1f, 0, 127);
}

inline TJBox_Int32 toNote(TJBox_Value value)
{
  return toNote(toJBoxFloat64(value));
}

inline TJBox_Value fromNote(TJBox_Int32 value)
{
  return JBox_MakeNumber(value / 127.f);
}

inline TJBox_Int32 toGate(TJBox_Float64 value)
{
  return clamp<TJBox_Int32>(value * 127.f, 0, 127);
}

inline TJBox_Int32 toGate(TJBox_Value value)
{
  return toGate(toJBoxFloat64(value));
}

inline TJBox_Value fromGate(TJBox_Int32 value)
{
  return JBox_MakeNumber(value / 127.f);
}

inline TJBox_Float64 toUnipolarCV(TJBox_Float64 iValue)
{
  return clamp2(iValue / 2.0 + 0.5, MIN_CV_VALUE, MAX_CV_VALUE);
}

inline TJBox_Float64 toBipolarCV(TJBox_Float64 iValue)
{
  return clamp2(iValue * 2.0 - 1.0, MIN_CV_VALUE, MAX_CV_VALUE);
}

}

const TJBox_Int32 Note_Init_Value = 0;

/**
* Typedefs
*/
typedef TCVInSocket<Float64JBoxProperty> CVInSocket;
typedef JBoxProperty<TJBox_Int32, JBox::toNote, JBox::fromNote> NoteJBoxProperty;
using CVInNote = TCVInSocket<ReadOnlyJBoxProperty<TJBox_Int32, JBox::toNote>>;
using CVInGate = TCVInSocket<ReadOnlyJBoxProperty<TJBox_Int32, JBox::toGate>>;

typedef TCVOutSocket<Float64JBoxProperty> CVOutSocket;
using CVOutNote = TCVOutSocket<WriteOnlyJBoxProperty<TJBox_Int32, JBox::fromNote>>;
using CVOutGate = TCVOutSocket<WriteOnlyJBoxProperty<TJBox_Int32, JBox::fromGate>>;

/**
 * This class implements the common behavior when a custom property has an equivalent CV In socket.
 *
 * When CV is connected, any change in the property itself will override the CV value. */
template<typename jbox_property_type, typename cv_property_type = jbox_property_type>
class CVInProperty
{
public:
  using value_type = typename jbox_property_type::value_type;
  using cv_mapper_type = std::function<value_type(typename cv_property_type::value_type)>;

public:
  /**
   * Constructor expecting the full path to the property itself (ex: `/custom_properties/my_prop`) and
   * the name of the CV In socket (ex: `my_prop_cv`). */
  CVInProperty(char const *iPropertyPath, char const *iCVInSocketName) :
    fProperty{iPropertyPath},
    fCVInSocket{iCVInSocketName}
  {}

  /**
   * @return the value of the property */
  inline value_type getValue() const { return fCurrentValue; }

  /**
   * Registers both the property (with the provided tag) and the cv in socket */
  void registerForUpdate(IJBoxPropertyManager &iMgr, TJBox_Tag iPropertyTag)
  {
    iMgr.registerForUpdate(fProperty, iPropertyTag);
    fCVInSocket.registerForUpdate(iMgr);
  }

  /**
   * Needs to be called after the motherboard was updated to "compute" the new current value
   *
   * @param iCVInValueMapper optional mapper to adapt the CV In value (for example, the meaning could change
   *                         based on other parameters, like a unipolar/bipolar switch)
   * @return `true` if the value has changed since the previous time this method was called
   */
  bool afterMotherboardUpdate(cv_mapper_type iCVInValueMapper = staticCast);

private:
  static constexpr value_type staticCast(typename cv_property_type::value_type v) { return static_cast<value_type>(v); }

private:
  jbox_property_type fProperty;
  TCVInSocket<cv_property_type> fCVInSocket;

  value_type fCurrentValue{};
  value_type fPropertyValue{};
  typename cv_property_type::value_type fCVInValue{};
  bool fCVInConnected{};
};

//------------------------------------------------------------------------
// CVInProperty::afterMotherboardUpdate
//------------------------------------------------------------------------
template<typename jbox_property_type, typename cv_property_type>
bool CVInProperty<jbox_property_type, cv_property_type>::afterMotherboardUpdate(cv_mapper_type iCVInValueMapper)
{
  auto previousValue = fCurrentValue;

  if(fProperty.updatePreviousValueOnChange(fPropertyValue))
  {
    fCurrentValue = fPropertyValue;
    if(fCVInSocket.isConnected())
    {
      fCVInConnected = true;
      fCVInValue = fCVInSocket.getValue();
    }
  }
  else
  {
    if(fCVInConnected) // was cv connected
    {
      if(fCVInSocket.isConnected()) // cv still connected
      {
        if(cv_property_type::updatePreviousValueOnChange(fCVInSocket.getValue(), fCVInValue))
        {
          fCurrentValue = iCVInValueMapper(fCVInValue);
        }
      }
      else // cv no longer connected
      {
        fCVInConnected = false;
        fCurrentValue = fPropertyValue;
      }
    }
    else // cv was not connected
    {
      if(fCVInSocket.isConnected()) // cv newly connected
      {
        fCVInConnected = true;
        fCVInValue = fCVInSocket.getValue();
        fCurrentValue = iCVInValueMapper(fCVInValue);
      }
    }
  }

  return previousValue != fCurrentValue;
}

#endif //__PongasoftCommon_CVSocket_h__
