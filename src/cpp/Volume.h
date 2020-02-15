//
// Created by Yan Pujante on 6/11/15.
//


#ifndef __Volume_H_
#define __Volume_H_

#include "JBoxProperty.h"
#include <cmath>

TJBox_Float32 const Volume_Init_Value = 1.0f;

inline TJBox_Float32 toVolumeCube(TJBox_Value value)
{
  // Volume [0..1]
  TJBox_Float32 vol = JBox::toJBoxFloat32(value);

  // shortcut... unity
  if(std::fabs(vol - 0.7f) < 1e-5)
  {
    return Volume_Init_Value;
  }

  const TJBox_Float32 correctedVolume = vol / 0.7f;
  TJBox_Float32 targetGain = correctedVolume * correctedVolume * correctedVolume;

  return targetGain;
}

inline TJBox_Value fromVolumeCube(TJBox_Float32 value)
{
  // (value ^ 1/3) * 0.7f
  return JBox_MakeNumber(std::pow(value, 1.0/3) * 0.7f);
}

typedef JBoxProperty<TJBox_Float32, toVolumeCube, fromVolumeCube> VolumeCubeJBoxProperty;

inline TJBox_Float64 toVolume(TJBox_AudioSample audioSample)
{
  return std::log10(audioSample) * 20;
}

class VolumeState
{
public:
  VolumeState(TJBox_Float32 iVolume, bool iFilterOn): fVolume(iVolume), fFilterOn(iFilterOn) {};

  inline void update(const VolumeState &rhs)
  {
    fVolume = rhs.fVolume;
    fFilterOn = rhs.fFilterOn;
  }

  bool adjustVolume(TJBox_Float32 iTargetVolume)
  {
    TJBox_Float32 previousVolume = fVolume;

    if(!fFilterOn)
    {
      fVolume = iTargetVolume;
    }
    else
    {
      // Filter changes to avoid nasty sounds.
      if(std::abs(fVolume - iTargetVolume) < 0.01f)
      {
        fVolume = iTargetVolume;
      }
      else
      {
        fVolume += (iTargetVolume - fVolume) / 100.0f;
        //JBOX_TRACE((std::string("volume adjusted... ") + std::to_string(previousVolume) + " to " + std::to_string(fVolume) + " target=" + std::to_string(iTargetVolume)).c_str());
      }
    }

    return previousVolume != fVolume;
  }

  inline TJBox_Float32 getVolume() const { return fVolume; }

  inline void setFilterOn(bool iFilterOn) { fFilterOn = iFilterOn; }

private:
  TJBox_Float32 fVolume;
  bool fFilterOn;
};

#endif //__Volume_H_
