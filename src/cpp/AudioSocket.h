//
// Created by Yan Pujante on 3/16/15.
//

#ifndef __PongasoftCommon_AudioSocket_h__
#define __PongasoftCommon_AudioSocket_h__


#include "JBoxPropertyManager.h"
#include "JBoxProperty.h"
#include "AudioBuffer.h"

/**
* AudioSocket
*/
class AudioSocket
{
public:
  AudioSocket(char const *iSocketPath);

  virtual void registerForUpdate(IJBoxPropertyManager &manager) = 0;

  inline TJBox_Bool isConnected() const
  {
    return fConnected.getValue();
  }

protected:
  JBoxObject iSocketObject;
  BooleanJBoxProperty fConnected;
};

/**
* AudioInSocket
*/
class AudioInSocket : public AudioSocket
{
public:
  AudioInSocket(char const *iSocketName) :
    AudioSocket(jbox::PropertyPath::printf("/audio_inputs/%s", iSocketName))
  {
  }

  virtual void registerForUpdate(IJBoxPropertyManager &manager);

  template<int size>
  void readAudio(TAudioBuffer<size> &iAudioBuffer) const
  {
    iAudioBuffer.readAudio(JBox_LoadMOMPropertyByTag(iSocketObject.fObjectRef, kJBox_AudioInputBuffer));
  }
};

/**
* AudioOutSocket
*/
class AudioOutSocket : public AudioSocket
{
public:
  explicit AudioOutSocket(char const *iSocketName) :
    AudioSocket(jbox::PropertyPath::printf("/audio_outputs/%s", iSocketName))
  {
  }

  virtual void registerForUpdate(IJBoxPropertyManager &manager);

  template<int size>
  void writeAudio(TAudioBuffer<size> const &iAudioBuffer) const
  {
    iAudioBuffer.writeAudio(JBox_LoadMOMPropertyByTag(iSocketObject.fObjectRef, kJBox_AudioOutputBuffer));
  }

};

/**
* StereoPair
*/
class StereoPair
{
public:
  virtual void registerForUpdate(IJBoxPropertyManager &manager);

  virtual AudioSocket &getLeftSocket() = 0;

  virtual AudioSocket &getRightSocket() = 0;

  virtual AudioSocket const &getLeftSocket() const = 0;

  virtual AudioSocket const &getRightSocket() const = 0;

  inline TJBox_Bool isConnected() const
  {
    return getLeftSocket().isConnected() || getRightSocket().isConnected();
  }
};

/**
* StereoInPair
*/
class StereoInPair : public StereoPair
{
public:
  StereoInPair(char const *iLeftSocketName, char const *iRightSocketName) :
    fLeftSocket(iLeftSocketName), fRightSocket(iRightSocketName)
  {
  }

  explicit StereoInPair(char const *iSocketName) :
    fLeftSocket(jbox::PropertyName::printf("%sLeft", iSocketName)),
    fRightSocket(jbox::PropertyName::printf("%sRight", iSocketName))
  {
  }


  virtual AudioSocket &getLeftSocket();

  virtual AudioSocket &getRightSocket();

  virtual AudioSocket const &getLeftSocket() const;

  virtual AudioSocket const &getRightSocket() const;

  template<int size>
  void readAudio(TStereoAudioBuffer<size> &iStereoAudioBuffer) const
  {
    fLeftSocket.readAudio(iStereoAudioBuffer.fLeftAudioBuffer);
    fRightSocket.readAudio(iStereoAudioBuffer.fRightAudioBuffer);
  }

  AudioInSocket fLeftSocket;
  AudioInSocket fRightSocket;
};

/**
* StereoOutPair
*/
class StereoOutPair : public StereoPair
{
public:
  StereoOutPair(char const *iLeftSocketName, char const *iRightSocketName) :
    fLeftSocket(iLeftSocketName), fRightSocket(iRightSocketName)
  {
  }

  explicit StereoOutPair(char const *iSocketName) :
    fLeftSocket(jbox::PropertyName::printf("%sLeft", iSocketName)),
    fRightSocket(jbox::PropertyName::printf("%sRight", iSocketName))
  {
  }


  virtual AudioSocket &getLeftSocket();

  virtual AudioSocket &getRightSocket();

  virtual AudioSocket const &getLeftSocket() const;

  virtual AudioSocket const &getRightSocket() const;

  template<int size>
  void writeAudio(TStereoAudioBuffer<size> const &iStereoAudioBuffer) const
  {
    fLeftSocket.writeAudio(iStereoAudioBuffer.fLeftAudioBuffer);
    fRightSocket.writeAudio(iStereoAudioBuffer.fRightAudioBuffer);
  }

  AudioOutSocket fLeftSocket;
  AudioOutSocket fRightSocket;
};

#endif //__PongasoftCommon_AudioSocket_h__
