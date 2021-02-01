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
template <typename T>
class TCVSocket
{
public:
  explicit TCVSocket(char const *iSocketPath):
    iSocketObject(iSocketPath), fConnected(iSocketObject, "connected"), fValue(iSocketObject, "value") {}

  virtual void registerForUpdate(IJBoxPropertyManager &manager) = 0;

  inline TJBox_Bool isConnected() const { return fConnected.getValue(); }

  inline bool isNewlyConnected(TCVSocket<T> const &previous) const
  {
    return !previous.isConnected() && isConnected();
  }

protected:
  JBoxObject iSocketObject;
  BooleanJBoxProperty fConnected;
  T fValue;
};

/**
* TCVInSocket
*/
template <typename T>
class TCVInSocket: public TCVSocket<T>
{
public:
  explicit TCVInSocket(char const *iSocketName) :
    TCVSocket<T>(jbox::ObjectPath::printf("/cv_inputs/%s", iSocketName))
  {};

  virtual void registerForUpdate(IJBoxPropertyManager &manager) {
    manager.registerForUpdate(this->fConnected, kJBox_CVInputConnected);
    manager.registerForUpdate(this->fValue, kJBox_CVInputValue);
  };

  inline typename T::value_type getValue() const { return this->fValue.getValue(); }

  inline bool isSameValue(TCVInSocket<T> const &other) const
  {
    return this->isConnected() == other.isConnected() && getValue() == other.getValue();
  }

  inline bool isNotSameValue(TCVInSocket<T> const &other) const
  {
    return !isSameValue(other);
  }
};


/**
* TCVOutSocket
*/
template <typename T>
class TCVOutSocket: public TCVSocket<T>
{
public:
  explicit TCVOutSocket(char const *iSocketName) :
    TCVSocket<T>(jbox::ObjectPath::printf("/cv_outputs/%s", iSocketName))
  {};

  virtual void registerForUpdate(IJBoxPropertyManager &manager)
  {
    manager.registerForUpdate(this->fConnected, kJBox_CVOutputConnected);
    manager.registerForInit(this->fValue);
  }

  void initMotherboard(typename T::value_type iValue)
  {
    this->fValue.initMotherboard(iValue);
  }

  bool storeValueToMotherboardOnUpdate(typename T::value_type value)
  {
    return this->fValue.storeValueToMotherboardOnUpdate(value);
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

  inline TJBox_Int32 toGate(TJBox_Float64 value)
  {
    return clamp<TJBox_Int32>(value * 127.f, 0, 127);
  }

  inline TJBox_Int32 toNote(TJBox_Value value)
  {
    return toNote(toJBoxFloat64(value));
  }

  inline TJBox_Value fromNote(TJBox_Int32 value)
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
typedef TCVInSocket<NoteJBoxProperty> CVInNote;
typedef TCVOutSocket<Float64JBoxProperty> CVOutSocket;

#endif //__PongasoftCommon_CVSocket_h__
