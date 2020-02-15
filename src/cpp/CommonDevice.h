//
//  CommonDevice.h
//

#pragma once
#ifndef __pongasoft__CommonDevice__
#define __pongasoft__CommonDevice__

#include "Jukebox.h"

class CommonDevice
{
public:
  CommonDevice()
  {
    JBOX_TRACE("CommonDevice()");
  }

  virtual ~CommonDevice() {};

  /**
  * @brief	Main starting point for rendering audio
  **/
  virtual void renderBatch(const TJBox_PropertyDiff iPropertyDiffs[], TJBox_UInt32 iDiffCount) = 0;

};

#endif /* defined(__pongasoft__CommonDevice__) */
