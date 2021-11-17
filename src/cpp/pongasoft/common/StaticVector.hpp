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

#ifndef __PongasoftCommon_StaticVector_h__
#define __PongasoftCommon_StaticVector_h__

#include <type_traits>
#include <optional>
#include <algorithm>
#include <logging/logging.h>

#include "stl.h"

#if DEBUG
#define DNOEXCEPT_F
#else
#define DNOEXCEPT_F noexcept
#endif


namespace pongasoft::common {

/**
 * The point of this class is to behave like a `std::vector` while having a fixed and static size. Boundaries are
 * checked in `DEBUG` mode only.
 *
 * The api is supposed to be the same or similar to `std::vector` so refer to the `std::vector` documentation in most
 * cases.
 *
 * @note Because the underlying class is an array, when elements are "removed" from the array (ex by using `drop_back`),
 *       only the size is adjusted, the element itself is never removed/or deleted (which is not allowed during
 *       `JBox_Export_RenderRealtime` anyway). */
template<typename T, std::size_t N>
class StaticVector
{
  static_assert(N > 0, "StaticVector of size 0 not allowed");

public:

  using class_type = StaticVector<T, N>;
  using array_type = std::array<T, N>;
  using value_type = typename array_type::value_type;
  using reference = typename array_type::reference;
  using const_reference  = typename array_type::const_reference;
  using iterator = typename array_type::iterator;
  using const_iterator = typename array_type::const_iterator;
  using pointer = typename array_type::pointer;
  using const_pointer = typename array_type::const_pointer;
  using size_type = typename array_type::size_type;
  using difference_type = typename array_type::difference_type;
  using reverse_iterator = typename array_type::reverse_iterator;
  using const_reverse_iterator = typename array_type::const_reverse_iterator;

public:

  /*******************************//////////////*******************************/
  /*                             < Constructors >                             */
  /*******************************/////////////********************************/
  //! Default / empty
  constexpr StaticVector() DNOEXCEPT_F = default;

  //! `count` copies of `value`
  explicit constexpr StaticVector(size_type count, const T& value = T()) DNOEXCEPT_F { insert(begin(), count, value); }

  //! from initializer list `{...}`
  explicit constexpr StaticVector(std::initializer_list<T> l) DNOEXCEPT_F { insert(begin(), l); }

  //! from other container
  template<class InputIt>
  constexpr StaticVector(InputIt first, InputIt last) DNOEXCEPT_F { insert(begin(), first, last); }

  //! copy constructor
  template<std::size_t M = N>
  constexpr StaticVector(StaticVector<T, M> const &other) DNOEXCEPT_F { insert(begin(), other.begin(), other.end()); }

  //! copy constructor
  constexpr StaticVector(class_type const &other) DNOEXCEPT_F { insert(begin(), other.begin(), other.end()); }

  //! copy assignment
  constexpr class_type &operator=(class_type const &other) DNOEXCEPT_F { clear(); insert(begin(), other.begin(), other.end()); return *this; }

  //! copy assignment
  template<std::size_t M = N>
  constexpr class_type &operator=(StaticVector<T, M> const &other) DNOEXCEPT_F { clear(); insert(begin(), other.begin(), other.end()); return *this; }

  //! Deleting move constructor (cannot move objects during processing)
  StaticVector(class_type &&other) = delete;

  /*******************************///////////*******************************/
  /*                             < Iterators >                             */
  /*******************************//////////********************************/

  constexpr iterator begin() DNOEXCEPT_F { return std::begin(fArray); }
  constexpr const_iterator begin() const DNOEXCEPT_F { return std::begin(fArray); }
  constexpr iterator end() DNOEXCEPT_F { return begin() + fSize; }
  constexpr const_iterator end() const DNOEXCEPT_F { return begin() + fSize; }

  constexpr reverse_iterator rbegin() DNOEXCEPT_F { return reverse_iterator(end()); }
  constexpr const_reverse_iterator rbegin() const DNOEXCEPT_F { return const_reverse_iterator(end()); }
  constexpr reverse_iterator rend() DNOEXCEPT_F { return reverse_iterator(begin()); }
  constexpr const_reverse_iterator rend() const DNOEXCEPT_F { return const_reverse_iterator(begin()); }

  constexpr const_iterator cbegin() const DNOEXCEPT_F { return begin(); }
  constexpr const_iterator cend() const DNOEXCEPT_F { return end(); }
  constexpr const_reverse_iterator crbegin() const DNOEXCEPT_F { return rbegin(); }
  constexpr const_reverse_iterator crend() const DNOEXCEPT_F { return rend(); }

  /**************************////////////////*******************************/
  /*                        < Element access >                             */
  /**************************///////////////********************************/

  constexpr reference operator[](size_type pos) DNOEXCEPT_F { DCHECK_F(pos >= 0 && pos < fSize); return fArray[pos]; }
  constexpr const_reference operator[](size_type pos) const DNOEXCEPT_F { DCHECK_F(pos >= 0 && pos < fSize); return fArray[pos]; }

  constexpr reference back() DNOEXCEPT_F { DCHECK_F(!empty()); return fArray[fSize - 1]; }
  constexpr const_reference back() const DNOEXCEPT_F { DCHECK_F(!empty()); return fArray[fSize - 1]; }

  constexpr reference front() DNOEXCEPT_F { DCHECK_F(!empty()); return fArray[0]; }
  constexpr const_reference front() const DNOEXCEPT_F { DCHECK_F(!empty()); return fArray[0]; }

  constexpr pointer data() DNOEXCEPT_F { return &fArray[0]; }
  constexpr const_pointer data() const DNOEXCEPT_F { return &fArray[0]; }

  //! Shortcut to return `true` if there is an element satisfying the unary predicate
  template<class UnaryPredicate >
  constexpr bool contains_if(UnaryPredicate p) const DNOEXCEPT_F { return std::find_if(cbegin(), cend(), p) != cend(); }

  //! Shortcut to return `true` if the element is in this vector (uses `==`)
  constexpr bool contains(T const &value) const DNOEXCEPT_F { return std::find(cbegin(), cend(), value) != cend(); }

  /**************************//////////*******************************/
  /*                        < Capacity >                             */
  /**************************//////////*******************************/
  [[nodiscard]] constexpr bool empty() const DNOEXCEPT_F { return fSize == 0; }

  //! returns `true` if there is any element aka not empty
  [[nodiscard]] constexpr bool any() const DNOEXCEPT_F { return fSize > 0; }

  //! return `true` if there is no more room available (`push_back` would fail)
  [[nodiscard]] constexpr bool full() const DNOEXCEPT_F { return fSize == N; }

  [[nodiscard]] constexpr size_type size() const DNOEXCEPT_F { return fSize; }

  /**************************///////////*******************************/
  /*                        < Modifiers >                             */
  /**************************///////////*******************************/

  //------------------------------------------------------------------------
  // push = insert element
  //------------------------------------------------------------------------

  //! adds an element to the back (note: cheap!)
  constexpr class_type &push_back(const T& value) DNOEXCEPT_F { DCHECK_F(!full()); fArray[fSize++] = value; return *this; }

  //! adds an element at the position (pushes the elements that are after) (note: expensive!)
  constexpr class_type &push_at(size_t pos, const T& value) DNOEXCEPT_F { insert(begin() + pos, value); return *this; }

  //! adds an element to the front (pushes the elements that are after) (note: expensive!)
  constexpr class_type &push_front(const T& value) DNOEXCEPT_F { push_at(0, value); return *this; }

  //! same as std::vector::insert
  constexpr iterator insert(const_iterator pos, const T& value) DNOEXCEPT_F { return insert(pos, 1, value); }

  //! same as std::vector::insert
  constexpr iterator insert(const_iterator pos, size_type count, const T& value) DNOEXCEPT_F;

  //! same as std::vector::insert
  template< class InputIt >
  constexpr iterator insert(const_iterator pos, InputIt first, InputIt last) DNOEXCEPT_F;

  //! same as std::vector::insert
  constexpr iterator insert(const_iterator pos, std::initializer_list<T> list) DNOEXCEPT_F { return insert(pos, list.begin(), list.end()); }

  //------------------------------------------------------------------------
  // pop = remove element (and ignore removed element)
  //------------------------------------------------------------------------

  //! removes the element at the back (note: cheap!)
  constexpr class_type &pop_back() DNOEXCEPT_F { DCHECK_F(any()); fSize--; return *this; }

  //! removes the element at the given index
  constexpr class_type &pop_at(size_t pos) DNOEXCEPT_F { DCHECK_F(pos >= 0 && pos < fSize); erase(cbegin() + pos); return *this;  }

  //! removes the element at the front (note: expensive!)
  constexpr class_type &pop_front() DNOEXCEPT_F { pop_at(0); return *this; }

  //------------------------------------------------------------------------
  // drop = remove element and return removed element
  //------------------------------------------------------------------------

  //! like `pop_back` but returns the popped element (must not be empty)
  constexpr value_type drop_back() DNOEXCEPT_F { DCHECK_F(!empty()); return fArray[fSize-- - 1]; }

  //! like `pop_at` but returns the popped element (must not be empty)
  constexpr value_type drop_at(size_t pos) DNOEXCEPT_F { DCHECK_F(pos >= 0 && pos < fSize); value_type res = fArray[pos]; pop_at(pos); return res; }

  //! like `pop_front` but returns the popped element (must not be empty)
  constexpr value_type drop_front() DNOEXCEPT_F { value_type res = front(); pop_front(); return res; }

  //! erase the element at pos (note: it is an iterator NOT an index)
  constexpr iterator erase(const_iterator pos) DNOEXCEPT_F
  {
    // copied from std
    iterator __r = __const_iterator_cast(pos);
    DCHECK_F(__r >= begin() && __r < end());
    std::copy(pos + 1, this->cend(), __r);
    --fSize;
    return __r;
  }

  //! erase the element at pos when using reverse iterators (note: it is an iterator NOT an index)
  constexpr reverse_iterator erase(const_reverse_iterator pos) DNOEXCEPT_F {
    reverse_iterator __r = __const_iterator_cast(pos);
    erase(pos.base() - 1);
    return __r;
  }

  //! erase the elements in the range `[first, last)` (note: it is iterators NOT an indices)
  constexpr iterator erase(const_iterator first, const_iterator last) DNOEXCEPT_F
  {
    iterator __r = __const_iterator_cast(first);
    if(first == last)
      return __r;

    DCHECK_F(__r >= begin() && __r < end());
    DCHECK_F(last >= begin() && last <= end());
    DCHECK_F(first < last);

    // copied from std
    difference_type __d = last - first;
    std::copy(last, this->cend(), __r);
    fSize -= __d;
    return __r;
  }

  /**
   * Same as `std::remove + erase` but faster
   *
   * @return the number of elements dropped (0 means the element was not present) */
  constexpr int drop(T const &value) DNOEXCEPT_F
  {
    auto size = fSize;
    auto newEnd = std::remove(begin(), end(), value);
    if(newEnd != end())
      fSize = newEnd - begin();
    return size - fSize;
  }

  /**
   * Same as `std::remove_if + erase` but faster
   *
   * @return the number of elements dropped (0 means no elements satisfied the predicate) */
  template<class UnaryPredicate>
  constexpr int drop_if(UnaryPredicate p) DNOEXCEPT_F
  {
    auto size = fSize;
    auto newEnd = std::remove_if(begin(), end(), p);
    if(newEnd != end())
      fSize = newEnd - begin();
    return size - fSize;
  }

  /**
   * Same as `drop_if` but moves erased items into `result`
   *
   * @param result will contain the elements that match the predicate
   * @return the number of elements dropped (0 means no elements satisfied the predicate) */
  template<class OutputIt, class UnaryPredicate>
  constexpr int drop_if(OutputIt result, UnaryPredicate p) DNOEXCEPT_F
  {
    auto size = fSize;
    auto newEnd = stl::remove_if(begin(), end(), result, p);
    if(newEnd != end())
      fSize = newEnd - begin();
    return size - fSize;
  }

  //! Empties the vector (cheap)
  constexpr void clear() DNOEXCEPT_F { fSize = 0; }

private:
  // copied from std
  constexpr iterator __const_iterator_cast(const_iterator __p) DNOEXCEPT_F {
    DCHECK_F(__p >= begin() && __p <= end() && __p < std::end(fArray));
    return begin() + (__p - cbegin());
  }

  constexpr reverse_iterator __const_iterator_cast(const_reverse_iterator __p) DNOEXCEPT_F {
    return rbegin() + (__p - crbegin());
  }

private:
  T fArray[N]; // not initialized on purpose: starts empty!
  size_type fSize{};
};

//------------------------------------------------------------------------
// StaticVector::insert count/value
//------------------------------------------------------------------------
template<typename T, std::size_t N>
constexpr typename StaticVector<T, N>::iterator
StaticVector<T, N>::insert(StaticVector::const_iterator pos, StaticVector::size_type count, T const &value) DNOEXCEPT_F
{
  DCHECK_F(fSize + count <= N);
  iterator iter = __const_iterator_cast(pos);
  if(fSize == 0)
  {
    for(int i = 0; i < count; i++)
      push_back(value);
  }
  else
  {
    if(count > 0)
    {
      std::copy_backward(iter, end(), end() + count);
      auto p = iter;
      for(int i = 0; i < count; i++)
      {
        *p = value;
        ++p;
      }
      fSize += count;
    }
  }
  return iter;
}

//------------------------------------------------------------------------
// StaticVector::insert other iterator
//------------------------------------------------------------------------
template<typename T, std::size_t N>
template<class InputIt>
constexpr typename StaticVector<T, N>::iterator
StaticVector<T, N>::insert(StaticVector::const_iterator pos, InputIt first, InputIt last) DNOEXCEPT_F
{
  auto count = std::distance(first, last);
  DCHECK_F(fSize + count <= N);
  iterator iter = __const_iterator_cast(pos);
  if(fSize == 0)
  {
    while(first != last)
    {
      push_back(*first);
      ++first;
    }
  }
  else
  {
    if(count > 0)
    {
      std::copy_backward(iter, end(), end() + count);
      auto p = iter;
      while(first != last)
      {
        *p = *first;
        ++p;
        ++first;
      }
      fSize += count;
    }
  }
  return iter;
}

}

#endif
