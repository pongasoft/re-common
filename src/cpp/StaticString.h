/*
 * Copyright (c) 2021 pongasoft
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 *
 * @author Yan Pujante
 */

#pragma once

#ifndef __PongasoftCommon_StaticString_h__
#define __PongasoftCommon_StaticString_h__

#include <cstddef>
#include "fmt.h"

/**
 * An immutable array of characters representing a c-style, null terminated string (enforced by this class).
 *
 * The type `char_array_type` defines which underlying representation is being used:
 *
 * - in debug mode it uses an `std::array` thus making sure that we don't access elements not belonging to the array
 * - in release mode, it uses a `char[]` for maximum performance
 *
 *
 * @tparam N
 */
template<size_t N>
class StaticString
{
  // Since this class represents a c-style null terminated string, there must be at least one element in
  // the array (the '\0' element!)
  static_assert(N > 0);

public:

  // underlying type depending on debug/release
#if DEBUG
  template<size_t M>
  using char_array_type = std::array<char, M>;
#else
  template<size_t M>
  using char_array_type = char[M];
#endif

  /**
   * Creates an empty string */
  constexpr StaticString() noexcept
  {
    // we know that N > 0
    fString[0] = '\0';
  }

  /**
   * Use the provided c-style (null terminated) string to initialize itself (up to N characters!)
   *
   * @note NOT marked explicit on purpose... */
  constexpr StaticString(char const *iString) noexcept
  {
    auto first = std::begin(fString);
    auto last = std::end(fString);

    char c{};

    while(first != last && (c = *iString++) != 0)
      *first++ = c;

    if(first == last)
      first--;

    *first = '\0';
  }

  /**
   * Builds a string from the underlying type */
  template<size_t M>
  constexpr explicit StaticString(char_array_type<M> iString) noexcept
  {
    if constexpr (N == M)
      fString = std::move(iString);
    else
      std::copy(std::begin(iString), std::begin(iString) + std::min(N, M), std::begin(fString));

    // we make sure that no matter what, the array is properly null terminated (should be null terminated
    // properly, but in case it is not, this ensures that it is)
    fString[N - 1] = '\0';
  }

  /**
   * Access this class as a `char const *` to pass to other functions */
  constexpr char const *operator*() const { return std::begin(fString); }

  /**
   * Access this class as a `char const *` to pass to other functions (same API as `std::string`) */
  constexpr char const *c_str() const noexcept { return std::begin(fString); }

  /**
   * Implicit conversion to a `char const *` */
  constexpr operator char const *() const noexcept { return std::begin(fString); }

  /**
   * Creates an instance of this class by using the format and arguments provided.
   *
   * @see `fmt::printf` for details on format */
  template<typename... Args>
  constexpr static StaticString<N> printf(char const *iFormat, Args &&... args) noexcept;

  /**
   * Compares 2 static strings according to the rules of `strncmp`
   * @return `0` if equal, `<0` if this is less than `iOther` and `>0` if this is greater than `iOther` */
  template<size_t M>
  inline int compare(StaticString<M> const &iOther) const noexcept
  {
    return strncmp(c_str(), iOther.c_str(), std::min(N, M));
  }

private:
  char_array_type<N> fString;
};

template<size_t N, size_t M>
bool operator<(StaticString<N> const &lhs, StaticString<M> const &rhs)
{
  return lhs.compare(rhs) < 0;
}

template<size_t N, size_t M>
bool operator>(StaticString<N> const &lhs, StaticString<M> const &rhs)
{
  return rhs < lhs;
}

template<size_t N, size_t M>
bool operator<=(StaticString<N> const &lhs, StaticString<M> const &rhs)
{
  return !(rhs < lhs);
}

template<size_t N, size_t M>
bool operator>=(StaticString<N> const &lhs, StaticString<M> const &rhs)
{
  return !(lhs < rhs);
}


//------------------------------------------------------------------------
// StaticString<N>::printf implementation
//------------------------------------------------------------------------
template<size_t N>
template<typename... Args>
constexpr StaticString<N> StaticString<N>::printf(char const *iFormat, Args &&... args) noexcept
{
  {
    char_array_type<N> arr;
    fmt::printf(std::begin(arr), std::end(arr), iFormat, std::forward<Args>(args)...);
    return StaticString{arr};
  }
}

#endif //__PongasoftCommon_StaticString_h__
