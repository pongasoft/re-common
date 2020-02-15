//
// Created by Yan Pujante on 3/16/15.
//

#include <Jukebox.h>
#include "AudioSocket.h"

////////////////////////////////
// AudioSocket
////////////////////////////////

AudioSocket::AudioSocket(char const *iSocketPath):
  iSocketObject(iSocketPath), fConnected(iSocketObject, "connected")
{

}

////////////////////////////////
// AudioInSocket
////////////////////////////////

void AudioInSocket::registerForUpdate(IJBoxPropertyManager &manager)
{
  manager.registerForUpdate(fConnected, kJBox_AudioInputConnected);
}


////////////////////////////////
// AudioOutSocket
////////////////////////////////

void AudioOutSocket::registerForUpdate(IJBoxPropertyManager &manager)
{
  manager.registerForUpdate(fConnected, kJBox_AudioOutputConnected);
}

////////////////////////////////
// StereoPair
////////////////////////////////

void StereoPair::registerForUpdate(IJBoxPropertyManager &manager)
{
  getLeftSocket().registerForUpdate(manager);
  getRightSocket().registerForUpdate(manager);
}

////////////////////////////////
// StereoInPair
////////////////////////////////

AudioSocket &StereoInPair::getLeftSocket()
{
  return fLeftSocket;
}

AudioSocket &StereoInPair::getRightSocket()
{
  return fRightSocket;
}


AudioSocket const &StereoInPair::getLeftSocket() const
{
  return fLeftSocket;
}

AudioSocket const &StereoInPair::getRightSocket() const
{
  return fRightSocket;
}

////////////////////////////////
// StereoOutPair
////////////////////////////////

AudioSocket &StereoOutPair::getLeftSocket()
{
  return fLeftSocket;
}

AudioSocket &StereoOutPair::getRightSocket()
{
  return fRightSocket;
}


AudioSocket const &StereoOutPair::getLeftSocket() const
{
  return fLeftSocket;
}

AudioSocket const &StereoOutPair::getRightSocket() const
{
  return fRightSocket;
}
