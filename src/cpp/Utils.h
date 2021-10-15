//
// Created by Yan Pujante on 10/12/16.
//

#ifndef __PongasoftCommon_UTILS_H
#define __PongasoftCommon_UTILS_H


#include "JukeboxTypes.h"
#include <cmath>
#include <logging/logging.h>

namespace Utils {

/**
 * Make sure that the value remains within its bounds. The difference with clamp is that iFrom and iTo do not have
 * to be provided in any specific order.
 */
template <typename T, typename U>
static constexpr T clampRange(const U &iValue, const T &iFrom, const T &iTo)
{
  if(iFrom < iTo)
    return clamp2(iValue, iFrom, iTo);
  else
    return clamp2(iValue, iTo, iFrom);
}

/**
 * Util class to compute linear interpolation. Use SPLerp/DPLerp and mapValueSP/mapRangeSP (resp mapValueDP/mapRangeDP)
 * for convenience.
 *
 * Providing iX1 == iX2 will result in undefined behavior
 */
template <typename TFloat, typename X, typename Y>
class Lerp
{
public:
  constexpr Lerp() : fA{1}, fB{0} {}

  constexpr Lerp(X iX1, Y iY1, X iX2, Y iY2) :
    fA((static_cast<TFloat>(iY1 - iY2)) / (static_cast<TFloat>(iX1 - iX2))),
    fB(static_cast<TFloat>(iY1) - fA * static_cast<TFloat>(iX1)) {}

  /**
   * Shortcut for when x=0 => iY0 and x=1.0 => iY1
   */
  constexpr Lerp(Y iY0, Y iY1) : fA(static_cast<TFloat>(iY1 - iY0)), fB(static_cast<TFloat>(iY0)) {};

  constexpr Y computeY(X iX) const
  {
    return static_cast<Y>((static_cast<TFloat>(iX) * fA) + fB);
  }

  constexpr X computeX(Y iY) const
  {
    DCHECK_F(fA != 0);
    return static_cast<X>((static_cast<TFloat>(iY) - fB) / fA);
  }

  /**
   * Inspired by the `map` function in Processing language, another way to look at Lerp is to map a range of values
   * into another range: `[iFromLow, iFromHigh] -> [iToLow, iToHigh]`. Note that low can be greater than high: for
   * example you can map `[1, -1]` to the range `[0, height]` (display where 0 is at the top and height is the bottom)
   */
  static constexpr Lerp mapRange(X iFromLow, X iFromHigh, Y iToLow, Y iToHigh)
  {
    return Lerp(iFromLow, iToLow, iFromHigh, iToHigh);
  }

  /**
   * Inspired by the `map` function in Processing language, another way to look at Lerp is to map a range of values
   * into another range: `[iFromLow, iFromHigh] -> [iToLow, iToHigh]`. This function then returns the `iValue` mapped from
   * the first range into the second range. When `iClamp` is set to `true`, the incoming value will first be clamped
   * to the from range. ex:
   *
   * ```
   * // 15 is inside "from" range
   * mapValue(15, 10, 20, 100, 200); // returns 150
   *
   * // 5 is outside "from" range
   * mapValue(5, 10, 20, 100, 200, true); // returns 100
   * mapValue(5, 10, 20, 100, 200, false); // returns 50
   * ```
   */
  static constexpr Y mapValue(X iValue, X iFromLow, X iFromHigh, Y iToLow, Y iToHigh, bool iClamp = true)
  {
    // if the first range is empty (computation would be dividing by 0)
    if(iFromLow == iFromHigh)
      return iValue <= iFromLow ? iToLow : iToHigh;

    // if the second range is empty, no need for computation
    if(iToLow == iToHigh)
      return iToLow;

    if(iClamp)
    {
      iValue = clampRange(iValue, iFromLow, iFromHigh);
    }

    return Lerp(iFromLow, iToLow, iFromHigh, iToHigh).computeY(iValue);
  }

private:
  const TFloat fA;
  const TFloat fB;
};

//------------------------------------------------------------------------
// SPLerp - Single Precision Lerp (TJBox_Float32)
//------------------------------------------------------------------------
template<typename X, typename Y>
using SPLerpXY = Lerp<TJBox_Float32, X, Y>;

template<typename X>
using SPLerpX = Lerp<TJBox_Float32, X, TJBox_Float32>;

template<typename Y>
using SPLerpY = Lerp<TJBox_Float32, TJBox_Float32, Y>;

using SPLerp = Lerp<TJBox_Float32, TJBox_Float32, TJBox_Float32>;

/**
 * Convenient shortcut for single precision. See Lerp::mapRange */
template<typename X, typename Y>
static constexpr SPLerpXY<X, Y> mapRangeSPXY(X iFromLow, X iFromHigh, Y iToLow, Y iToHigh)
{
  return SPLerpXY<X, Y>(iFromLow, iToLow, iFromHigh, iToHigh);
}

template<typename Y>
constexpr auto mapRangeSPY = mapRangeSPXY<TJBox_Float32, Y>;

template<typename X>
constexpr auto mapRangeSPX = mapRangeSPXY<X, TJBox_Float32>;

constexpr auto mapRangeSP = mapRangeSPXY<TJBox_Float32, TJBox_Float32>;

/**
 * Convenient shortcut for single precision. See Lerp::mapValue */
template<typename X, typename Y>
static constexpr Y mapValueSPXY(X iValue, X iFromLow, X iFromHigh, Y iToLow, Y iToHigh, bool iClamp = true)
{
  return SPLerpXY<X, Y>::mapValue(iValue, iFromLow, iFromHigh, iToLow, iToHigh, iClamp);
}

/**
 * Convenient shortcut for single precision. See Lerp::mapValue */
template<typename Y>
static constexpr Y mapValueSPY(TJBox_Float32 iValue, TJBox_Float32 iFromLow, TJBox_Float32 iFromHigh, Y iToLow, Y iToHigh, bool iClamp = true)
{
  return SPLerpY<Y>::mapValue(iValue, iFromLow, iFromHigh, iToLow, iToHigh, iClamp);
}

/**
 * Convenient shortcut for single precision. See Lerp::mapValue */
template<typename X>
static constexpr TJBox_Float32 mapValueSPX(X iValue, X iFromLow, X iFromHigh, TJBox_Float32 iToLow, TJBox_Float32 iToHigh, bool iClamp = true)
{
  return SPLerpX<X>::mapValue(iValue, iFromLow, iFromHigh, iToLow, iToHigh, iClamp);
}

/**
 * Convenient shortcut for single precision. See Lerp::mapValue */
static constexpr TJBox_Float32 mapValueSP(TJBox_Float32 iValue, TJBox_Float32 iFromLow, TJBox_Float32 iFromHigh, TJBox_Float32 iToLow, TJBox_Float32 iToHigh, bool iClamp = true)
{
  return SPLerp::mapValue(iValue, iFromLow, iFromHigh, iToLow, iToHigh, iClamp);
}


//------------------------------------------------------------------------
// DPLerp - Double Precision Lerp (TJBox_Float64)
//------------------------------------------------------------------------
template<typename X, typename Y>
using DPLerpXY = Lerp<TJBox_Float64, X, Y>;

template<typename X>
using DPLerpX = Lerp<TJBox_Float64, X, TJBox_Float64>;

template<typename Y>
using DPLerpY = Lerp<TJBox_Float64, TJBox_Float64, Y>;

using DPLerp = Lerp<TJBox_Float64, TJBox_Float64, TJBox_Float64>;

/**
 * Convenient shortcut for TJBox_Float64 precision. See Lerp::mapRange */
template<typename X, typename Y>
static constexpr DPLerpXY<X, Y> mapRangeDPXY(X iFromLow, X iFromHigh, Y iToLow, Y iToHigh)
{
  return DPLerpXY<X, Y>(iFromLow, iToLow, iFromHigh, iToHigh);
}

template<typename Y>
constexpr auto mapRangeDPY = mapRangeDPXY<TJBox_Float64, Y>;

template<typename X>
constexpr auto mapRangeDPX = mapRangeDPXY<X, TJBox_Float64>;

constexpr auto mapRangeDP = mapRangeDPXY<TJBox_Float64, TJBox_Float64>;

/**
 * Convenient shortcut for single precision. See Lerp::mapValue */
template<typename X, typename Y>
static constexpr Y mapValueDPXY(X iValue, X iFromLow, X iFromHigh, Y iToLow, Y iToHigh, bool iClamp = true)
{
  return DPLerpXY<X, Y>::mapValue(iValue, iFromLow, iFromHigh, iToLow, iToHigh, iClamp);
}

/**
 * Convenient shortcut for single precision. See Lerp::mapValue */
template<typename Y>
static constexpr Y mapValueDPY(TJBox_Float64 iValue, TJBox_Float64 iFromLow, TJBox_Float64 iFromHigh, Y iToLow, Y iToHigh, bool iClamp = true)
{
  return DPLerpY<Y>::mapValue(iValue, iFromLow, iFromHigh, iToLow, iToHigh, iClamp);
}

/**
 * Convenient shortcut for single precision. See Lerp::mapValue */
template<typename X>
static constexpr TJBox_Float64 mapValueDPX(X iValue, X iFromLow, X iFromHigh, TJBox_Float64 iToLow, TJBox_Float64 iToHigh, bool iClamp = true)
{
  return DPLerpX<X>::mapValue(iValue, iFromLow, iFromHigh, iToLow, iToHigh, iClamp);
}

/**
 * Convenient shortcut for single precision. See Lerp::mapValue */
inline constexpr TJBox_Float64 mapValueDP(TJBox_Float64 iValue, TJBox_Float64 iFromLow, TJBox_Float64 iFromHigh, TJBox_Float64 iToLow, TJBox_Float64 iToHigh, bool iClamp = true)
{
  return DPLerp::mapValue(iValue, iFromLow, iFromHigh, iToLow, iToHigh, iClamp);
}


/**
 * Makes changing a value smooth when the filter is on.
 *
 * Usage:
 * - call `adjustValue` on every batch
 * - call `getValue` to get the current filtered value
 *
 * @tparam T
 */
template<typename T = TJBox_Float32, T precision = static_cast<T>(0.01)>
class FilteredValue
{
public:
  FilteredValue(T iOriginalValue, bool iFilterOn): fValue(iOriginalValue), fFilterOn(iFilterOn) {};

  inline void update(const FilteredValue &rhs)
  {
    fValue = rhs.fValue;
    fFilterOn = rhs.fFilterOn;
  }

  bool adjustValue(T iTargetValue)
  {
    T previousValue = fValue;

    if(!fFilterOn)
      fValue = iTargetValue;
    else
    {
      if(std::abs(fValue - iTargetValue) < precision)
        fValue = iTargetValue;
      else
        fValue += (iTargetValue - fValue) / static_cast<T>(100);
    }

    return previousValue != fValue;
  }

  inline T getValue() const { return fValue; }

  inline void setFilterOn(bool iFilterOn) { fFilterOn = iFilterOn; }

private:
  T fValue;
  bool fFilterOn;
};



};


#endif //__PongasoftCommon_UTILS_H
