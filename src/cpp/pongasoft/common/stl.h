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

#ifndef RE_COMMON_STL_H
#define RE_COMMON_STL_H

#include <algorithm>
#include <optional>

/**
 * Contains useful wrappers to common algorithm found in the std */
namespace pongasoft::common::stl {

/**
 * `std::remove_if` removes all elements satisfying the predicate from the range [first, last) and returns
 * a past-the-end iterator for the new end of the range. This version moves the discarded elements (the one which
 * satisfies the predicate) into `result`.
 *
 * @note This call is usually followed by a call to a container `erase` method as it only moves the elements
 *       but not change the size or `end` of the container!
 * @see `std::remove` for more details */
template <class ForwardIterator, class OutputIterator, class Predicate>
inline constexpr ForwardIterator remove_if(ForwardIterator first,
                                           ForwardIterator last,
                                           OutputIterator result,
                                           Predicate pred)
{
  first = std::find_if<ForwardIterator, typename std::add_lvalue_reference<Predicate>::type>(first, last, pred);

  if(first != last)
  {
    ForwardIterator i = first;

    // first one is coming from std::find_if
    *result = std::move(*i);

    while(++i != last)
    {
      if(pred(*i))
      {
        *result = std::move(*i);
        ++result;
      }
      else
      {
        *first = std::move(*i);
        ++first;
      }
    }
  }

  return first;
}

/**
 * Shortcut to check whether a predicate is satisfied by at least one element in the entire container (from begin
 * to end) in which case it returns `true`. `const` version.
 *
 * @see `std::find_if` for more details */
template<typename Container, typename Predicate>
inline constexpr bool contains_if(Container const &iContainer, Predicate iPredicate)
{
  return std::find_if(std::cbegin(iContainer), std::cend(iContainer), iPredicate) != std::cend(iContainer);
}

/**
 * Returns the first element (wrapped in a `std::optional`) of a container if the container is not empty otherwise
 * it returns `std::nullopt`
 *
 * @note Since `std::optional` does NOT accept references, unlike `iContainer.first()` which returns a reference
 *       to the first element, this call returns a copy of the first element. */
template<typename Container>
inline constexpr auto first_or_null(Container const &iContainer)
-> std::optional<typename std::remove_const_t<std::remove_reference_t<decltype(*std::begin(iContainer))>>>
{
  auto iter = std::cbegin(iContainer);
  if(iter == std::cend(iContainer))
    return std::nullopt;
  else
    return *iter;
}

/**
 * Similar to `std::find_if` but instead of returning an iterator to the first found element it returns the element
 * itself (wrapped in a `std::optional`) or `std::nullopt` when not found.
 *
 * @see `std::find_if` for more details */
template<typename Container, typename Predicate>
inline constexpr auto find_if_or_null(Container const &iContainer, Predicate iPredicate) -> decltype(first_or_null(iContainer))
{
  auto const iter = std::find_if(std::cbegin(iContainer), std::cend(iContainer), iPredicate);
  if(iter == std::cend(iContainer))
    return std::nullopt;
  else
    return *iter;
}

/**
 * Same as `std::for_each` for all elements of the container. */
template<typename Container, typename UnaryFunction>
inline constexpr UnaryFunction for_each(Container &iContainer, UnaryFunction iFunction)
{
  return std::for_each(std::begin(iContainer), std::end(iContainer), iFunction);
}

/**
 * Same as `std::for_each` for all elements of the container. */
template<typename Container, typename UnaryFunction>
inline constexpr UnaryFunction for_each(Container const &iContainer, UnaryFunction iFunction)
{
  return std::for_each(std::cbegin(iContainer), std::cend(iContainer), iFunction);
}

}

#endif //RE_COMMON_STL_H
