//
// Created by Yan Pujante on 6/5/15.
//


#ifndef __AudioBuffer_H_
#define __AudioBuffer_H_

#include "JukeboxTypes.h"
#include "Constants.h"
#include "Jukebox.h"
#include "Volume.h"
#include "XFade.h"

template <int size = kBatchSize>
class TAudioBuffer
{
public:
  typedef TAudioBuffer<size> class_type;

public:
  TAudioBuffer() {};

  constexpr int getSize() const { return size; }

  void readAudio(TJBox_Value const &audioValue)
  {
    JBox_GetDSPBufferData(audioValue, 0, size, fAudioBuffer);
  }

  void writeAudio(TJBox_Value const &audioValue) const
  {
    JBox_SetDSPBufferData(audioValue, 0, size, fAudioBuffer);
  }

  void accumulate(class_type const &rhs)
  {
    TJBox_AudioSample *lhsSample = fAudioBuffer;
    TJBox_AudioSample const *rhsSample = rhs.fAudioBuffer;

    for(int i = 0; i < size; ++i, lhsSample++, rhsSample++)
    {
      *lhsSample = *lhsSample + *rhsSample;
    }
  }

  void copy(class_type const &rhs)
  {
    TJBox_AudioSample *lhsSample = fAudioBuffer;
    TJBox_AudioSample const *rhsSample = rhs.fAudioBuffer;

    for(int i = 0; i < size; ++i, lhsSample++, rhsSample++)
    {
      *lhsSample = *rhsSample;
    }
  }

  void adjustGain(TJBox_Float32 iGain)
  {
    // no need to multiply by 1.0!
    if(iGain == Volume_Init_Value)
      return;

    for(TJBox_Int32 i = 0; i < size; i++)
    {
      fAudioBuffer[i] *= iGain;
    }
  }

  void clear()
  {
    for(TJBox_Int32 i = 0; i < size; i++)
    {
      fAudioBuffer[i] = 0;
    }
  }

  TJBox_AudioSample max() const
  {
    TJBox_AudioSample res = 0;

    for(TJBox_Int32 i = 0; i < size; i++)
    {
      TJBox_AudioSample sampleValue = fAudioBuffer[i];

      if(sampleValue < 0)
        sampleValue = -sampleValue;

      res = std::max(res, sampleValue);
    }

    return res;
  }

  /**
  ** Code imported from examples.
  **
  ** @brief		Checks if a buffer is completely silent.
  ** @details	Note that everything below kJBox_SilentThreshold should be considered silent.
  **/
  bool isSilent() const
  {
    bool silent = true;
    for(TJBox_Int32 i = 0; i < size; i++)
    {
      TJBox_AudioSample sampleValue = fAudioBuffer[i];
      if(sampleValue < kJBox_SilentThreshold)
      {
        sampleValue = -sampleValue;
      }
      if(sampleValue > kJBox_SilentThreshold)
      {
        silent = false;
        break;
      }
    }
    return silent;
  }

public:
  TJBox_AudioSample fAudioBuffer[size];
};

template <int size = kBatchSize>
class TStereoAudioBuffer
{
public:
  typedef TStereoAudioBuffer<size> class_type;

public:
  TStereoAudioBuffer() {}

public:
  constexpr int getSize() const { return size; }

  inline void accumulate(class_type const &rhs)
  {
    fLeftAudioBuffer.accumulate(rhs.fLeftAudioBuffer);
    fRightAudioBuffer.accumulate(rhs.fRightAudioBuffer);
  }

  inline void adjustGain(TJBox_Float32 iGain)
  {
    fLeftAudioBuffer.adjustGain(iGain);
    fRightAudioBuffer.adjustGain(iGain);
  }

  inline void clear()
  {
    fLeftAudioBuffer.clear();
    fRightAudioBuffer.clear();
  }

  inline void copy(class_type const &rhs)
  {
    fLeftAudioBuffer.copy(rhs.fLeftAudioBuffer);
    fRightAudioBuffer.copy(rhs.fRightAudioBuffer);
  }

  inline TJBox_AudioSample max() const
  {
    return std::max(fLeftAudioBuffer.max(), fRightAudioBuffer.max());
  }

  inline bool isSilent()
  {
    return fLeftAudioBuffer.isSilent() && fRightAudioBuffer.isSilent();
  }

  void xFade(XFade<size> const &iXFade, class_type const &other)
  {
    xFade(iXFade, *this, other);
  }

  void xFade(XFade<size> const &iXFade, class_type const &sab1, class_type const &sab2)
  {
    const TJBox_Float64 *xFadeFunction = iXFade.fXFadeFunction;
    const TJBox_Float64 *vReverseXFadeFunction = &iXFade.fXFadeFunction[size - 1];

    TJBox_AudioSample* vLeftAudioBuffer = fLeftAudioBuffer.fAudioBuffer;
    TJBox_AudioSample* vRightAudioBuffer = fRightAudioBuffer.fAudioBuffer;

    const TJBox_AudioSample *vLAB1 = sab1.fLeftAudioBuffer.fAudioBuffer;
    const TJBox_AudioSample *vRAB1 = sab1.fRightAudioBuffer.fAudioBuffer;

    const TJBox_AudioSample *vLAB2 = sab2.fLeftAudioBuffer.fAudioBuffer;
    const TJBox_AudioSample *vRAB2 = sab2.fRightAudioBuffer.fAudioBuffer;

    for(int i = 0; i < size; i++)
    {
      TJBox_Float64 vXFade = *xFadeFunction++;
      TJBox_Float64 vReverseXFade = *vReverseXFadeFunction--;

      *vLeftAudioBuffer++ =
        (TJBox_AudioSample) ((*vLAB1++ * vXFade) + (*vLAB2++ * vReverseXFade));
      *vRightAudioBuffer++ =
        (TJBox_AudioSample) ((*vRAB1++ * vXFade) + (*vRAB2++ * vReverseXFade));
    }
  }

public:
  TAudioBuffer<size> fLeftAudioBuffer;
  TAudioBuffer<size> fRightAudioBuffer;
};

typedef TAudioBuffer<kBatchSize> AudioBuffer;
typedef TStereoAudioBuffer<kBatchSize> StereoAudioBuffer;

#endif //__AudioBuffer_H_
