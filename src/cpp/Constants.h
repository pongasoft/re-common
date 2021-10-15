#pragma once
#ifndef __pongasoft__Constants__
#define __pongasoft__Constants__

#include "JukeboxTypes.h"
#include <stddef.h> // to define size_t
#include <cmath>
#include <limits>

#define JBOX_ASSERT_RANGE(v, min, max) JBOX_ASSERT((v) >= (min) && (v) <= (max))

const TJBox_Int32 kBatchSize = 64;

// max size for a full path property name (ex: /custom_properties/my_prop) (not including terminating 0)
const size_t kMaxPropertyPathLen = kJBox_MaxPropertyNameLen + kJBox_MaxObjectNameLen;

// If you want to define them, make them also the type TJBox_Bool (unsigned char), with FALSE = 0, TRUE = 1.

const TJBox_Bool TJbox_TRUE = 1;
const TJBox_Bool TJbox_FALSE = 0;

const TJBox_Int32 kMidiC0 = 24;
const TJBox_Int32 kMidiC1 = kMidiC0 + 12;
const TJBox_Int32 kMidiC2 = kMidiC1 + 12;
const TJBox_Int32 kMidiC3 = kMidiC2 + 12;
const TJBox_Int32 kMidiC4 = kMidiC3 + 12;

const TJBox_Float64 MIN_CV_VALUE = -10000;
const TJBox_Float64 MAX_CV_VALUE = 10000;

constexpr auto MAX_TJbox_Float64 = std::numeric_limits<TJBox_Float64>::max();

template <typename T>
static constexpr T clamp(TJBox_Float64 value, int lower, int upper)
{
  return static_cast<T>(value < lower ? lower : (value > upper ? upper : value));
}

/**
 * Make sure that the value remains within its bounds
 *
 * @param iValue the value to clamp between `iLower` and `iUpper`
 * @param iLower the lower bound (must be <= iUpper)
 * @param iUpper the upper bound (must be >= iLower)
 */
template <typename T, typename U>
static constexpr T clamp2(const U &iValue, const T &iLower, const T &iUpper)
{
  auto v = static_cast<T>(iValue);
  return v < iLower ? iLower : (v > iUpper ? iUpper : v);
}

inline static double batchDurationMillis(int iSampleRate)
{
  return kBatchSize * 1000.0 / iSampleRate;
}

inline static int batchCountToFitDuration(int iSampleRate, long iDurationMillis)
{
  if(iDurationMillis == 0)
    return 0;

  return (int) ceil(iDurationMillis / batchDurationMillis(iSampleRate));
}

/**
 * Equals with 5 digits precision */
template <typename T>
inline static bool equals5DP(T const &v1, T const &v2)
{
  return std::fabs(v1 - v2) < 1e-5;
}

#endif /* define(__pongasoft__Constants__) */
