//
// Created by Yan Pujante on 10/12/16.
//

#ifndef __PongasoftCommon_UTILS_H
#define __PongasoftCommon_UTILS_H


#include "JukeboxTypes.h"
#include <cmath>

namespace Utils {

/**
 * Util class to compute linear interpolation. Note that assembly code totally removes this class which is great!
 * ex: Utils::Lerp(MAX_ZOOM_FACTOR_Y, 1.0).compute(fMotherboard.fPropZoomFactorY.getValue());
 */
class Lerp
{
public:
  Lerp(TJBox_Float64 iX1, TJBox_Float64 iY1, TJBox_Float64 iX2, TJBox_Float64 iY2) :
    fA((iY1 - iY2) / (iX1 - iX2)),
    fB(iY1 - fA * iX1)
  {}

  /**
   * Shortcut for when x=0 => iY0 and x=1.0 => iY1
   */
  Lerp(TJBox_Float64 iY0, TJBox_Float64 iY1) : fA(iY1 - iY0), fB(iY0) {};

  inline TJBox_Float64 compute(TJBox_Float64 iX) const
  {
    return (iX * fA) + fB;
  }

  /**
   * Inspired by the `map` function in Processing language, another way to look at Lerp is to map a range of values
   * into another range: `[iFromLow, iFromHigh] -> [iToLow, iToHigh]`. Note that low can be greater than high: for
   * example you can map `[1, -1]` to the range `[0, height]` (display where 0 is at the top and height is the bottom)
   */
  static inline Lerp mapRange(TJBox_Float64 iFromLow, TJBox_Float64 iFromHigh,
                              TJBox_Float64 iToLow, TJBox_Float64 iToHigh)
  {
    return Lerp(iFromLow, iToLow, iFromHigh, iToHigh);
  }

private:
  const TJBox_Float64 fA;
  const TJBox_Float64 fB;
};

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
