//
// Created by Yan Pujante on 6/13/15.
//


#ifndef __XFade_H_
#define __XFade_H_

#include "JukeboxTypes.h"
#include "Jukebox.h"

template <int size>
class XFade
{
public:
  TJBox_Float64 fXFadeFunction[size];
};

/*
  cross fade linear
  formula is 0.5 * (1 + t) with t[-1, +1]
  a = 2 / N (sampling over N samples (N = 64))
  => t = -1 + (n * a) with n from 0 to N
  => formula is 0.5 * (1 + t) = 0.5 * (1 + -1 + n * a) = 0.5 * a * n = n / N
  (0..N).each { n -> println "$n -> ${Math.sqrt(n/N)}" }
*/

template <int size>
class LinearXFade: public XFade<size>
{
public:
  LinearXFade()
  {
    for(int i = 0; i < size; i++)
    {
      this->fXFadeFunction[i] = (double) i / size;
    }
  }
};


#endif //__XFade_H_
