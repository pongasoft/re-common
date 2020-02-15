//
// Created by Yan Pujante on 10/12/16.
//

#ifndef __PongasoftCommon_UTILS_H
#define __PongasoftCommon_UTILS_H


#import "JukeboxTypes.h"

namespace Utils
{
  /**
   * Util class to compute linear interpolation. Note that assembly code totally removes this class which is great!
   * ex: Utils::Lerp(MAX_ZOOM_FACTOR_Y, 1.0).compute(fMotherboard.fPropZoomFactorY.getValue());
   */
  class Lerp
  {
  public:
    Lerp(TJBox_Float64 iX1, TJBox_Float64 iY1, TJBox_Float64 iX2, TJBox_Float64 iY2) : fA((iY1 - iY2) / (iX1 - iX2)), fB(iY1 - fA * iX1) {}

    /**
     * Shortcut for when x=0 => iY0 and x=1.0 => iY1
     */
    Lerp(TJBox_Float64 iY0, TJBox_Float64 iY1) : fA(iY1 - iY0), fB(iY0) {};

    inline TJBox_Float64 compute(TJBox_Float64 iX) const
    {
      return (iX * fA) + fB;
    }

  private:
    const TJBox_Float64 fA;
    const TJBox_Float64 fB;
  };

};


#endif //__PongasoftCommon_UTILS_H
