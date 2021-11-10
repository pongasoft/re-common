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

#include <pongasoft/common/StaticVector.hpp>
#include <vector>
#include <gtest/gtest.h>
#include <ostream>

namespace pongasoft::common::Test {

namespace test_StaticVector {

struct S {

  S() = default;
  S(int i) : x{i} {}

  int x{};

  bool operator==(S const &rhs) const
  {
    return x == rhs.x;
  }

  bool operator!=(S const &rhs) const
  {
    return !(rhs == *this);
  }

  friend std::ostream &operator<<(std::ostream &os, S const &s)
  {
    os << "S{" << s.x << "}";
    return os;
  }
};

}

using S = test_StaticVector::S;

template<typename T, std::size_t N>
std::vector<T> toVector(StaticVector<T, N> const &iStaticVector)
{
  return {iStaticVector.cbegin(), iStaticVector.cend()};
}

template<std::size_t N, typename... Args>
StaticVector<S, N> SV_S(Args&& ...iValues)
{
//  static_assert(sizeof...(iValues) <= N);
  return StaticVector<S, N>{S(iValues)...};
}

template<typename... Args>
std::vector<S> V_S(Args&& ...iValues)
{
  return std::vector<S>{S(iValues)...};
}


template<typename T, std::size_t N>
void check(StaticVector<T, N> const &iLeft, std::vector<T> const &iRight)
{
  auto left = std::vector<T>(iLeft.cbegin(), iLeft.cend());
  EXPECT_EQ(left, iRight);
  if(left != iRight)
    throw std::runtime_error("check failed");
}

auto even_s = [](auto s) { return s.x % 2 == 0; };
auto gt100_s = [](auto s) { return s.x > 100; };

// StaticVector - S_constructors
TEST(StaticVector, S_constructors)
{
  loguru::init_for_test("S_constructors");

  // default constructor
  check(StaticVector<S, 2>(), V_S());

  // count
  check(StaticVector<S, 2>(0), V_S()); // count (0)
  check(StaticVector<S, 2>(1), V_S(0)); // count (1)
  check(StaticVector<S, 2>(1, S(10)), V_S(10)); // count (1)
  check(StaticVector<S, 2>(2, S(10)), V_S(10, 10)); // count (2)
  ASSERT_THROW((StaticVector<S, 2>(3, S(10))), std::runtime_error); // count (3) error

  // initializer list
  check(SV_S<2>(10), V_S(10));
  check(SV_S<2>(10, 20), V_S(10, 20));
  ASSERT_THROW(SV_S<2>(10, 20, 30), std::runtime_error); // initializer list error

  // define a vector
  auto vec = V_S(10, 20, 30, 40, 50);

  // other container
  check(StaticVector<S, 2>(vec.begin(), vec.begin()), V_S()); // other container (empty)
  check(StaticVector<S, 2>(vec.begin() + 3, vec.begin() + 4), V_S(40)); // other container 1 value
  check(StaticVector<S, 2>(vec.begin() + 3, vec.end()), V_S(40, 50)); // other container 2 values
  ASSERT_THROW((StaticVector<S, 2>(vec.begin() + 2, vec.end())), std::runtime_error); // other container error

  // copy constructor
  check(StaticVector<S, 2>(SV_S<1>(10)), V_S(10)); // other container (empty)
  check(StaticVector<S, 2>(SV_S<2>(10)), V_S(10)); // other container (1)
  check(StaticVector<S, 2>(SV_S<2>(10, 20)), V_S(10, 20)); // other container (2)
  ASSERT_THROW((StaticVector<S, 2>(SV_S<3>(10, 20, 30))), std::runtime_error); // other container error
}

// StaticVector - S_iterators
TEST(StaticVector, S_iterators)
{
  loguru::init_for_test("S_iterators");

  {
    StaticVector<S, 2> empty{};

    ASSERT_EQ(empty.begin(), empty.end());
    ASSERT_EQ(empty.cbegin(), empty.cend());
    ASSERT_EQ(empty.rbegin(), empty.rend());
    ASSERT_EQ(empty.crbegin(), empty.crend());

    auto vec1 = SV_S<2>(10);

    ASSERT_EQ(vec1.begin() + 1, vec1.end());
    ASSERT_EQ(vec1.cbegin() + 1, vec1.cend());
    ASSERT_EQ(*vec1.rbegin(), *vec1.begin());
    ASSERT_EQ(*vec1.crbegin(), *vec1.cbegin());

    auto vec2 = SV_S<2>(10, 20);

    ASSERT_EQ(vec2.begin() + 2, vec2.end());
    ASSERT_EQ(*vec2.begin(), S(10));
    ASSERT_EQ(*(vec2.begin() + 1), S(20));
    ASSERT_EQ(*(vec2.end() - 1), S(20));
    ASSERT_EQ(vec2.cbegin() + 2, vec2.cend());
    ASSERT_EQ(*vec2.rbegin(), *(vec2.end() - 1));
    ASSERT_EQ(*vec2.crbegin(), *(vec2.cend() - 1));
  }

  {
    const StaticVector<S, 2> empty{};

    ASSERT_EQ(empty.begin(), empty.end());
    ASSERT_EQ(empty.cbegin(), empty.cend());
    ASSERT_EQ(empty.rbegin(), empty.rend());
    ASSERT_EQ(empty.crbegin(), empty.crend());


    auto const vec1 = SV_S<2>(10);

    ASSERT_EQ(vec1.begin() + 1, vec1.end());
    ASSERT_EQ(vec1.cbegin() + 1, vec1.cend());
    ASSERT_EQ(*vec1.rbegin(), *vec1.begin());
    ASSERT_EQ(*vec1.crbegin(), *vec1.cbegin());

    auto const vec2 = SV_S<2>(10, 20);

    ASSERT_EQ(vec2.begin() + 2, vec2.end());
    ASSERT_EQ(*vec2.begin(), S(10));
    ASSERT_EQ(*(vec2.begin() + 1), S(20));
    ASSERT_EQ(*(vec2.end() - 1), S(20));
    ASSERT_EQ(vec2.cbegin() + 2, vec2.cend());
    ASSERT_EQ(*vec2.rbegin(), *(vec2.end() - 1));
    ASSERT_EQ(*vec2.crbegin(), *(vec2.cend() - 1));
  }
}

// StaticVector - S_element_access
TEST(StaticVector, S_element_access)
{
  loguru::init_for_test("S_element_access");

  {
    StaticVector<S, 2> empty{};

    ASSERT_THROW(empty[0], std::runtime_error);
    ASSERT_THROW(empty[1], std::runtime_error);
    ASSERT_THROW(empty.front(), std::runtime_error);
    ASSERT_THROW(empty.back(), std::runtime_error);
    ASSERT_FALSE(empty.contains(S(10)));
    ASSERT_FALSE(empty.contains_if(even_s));
    ASSERT_EQ(empty.data(), empty.begin());

    auto vec = SV_S<10>(1, 2, 3, 4, 5, 6);
    ASSERT_EQ(vec[0], S(1));
    ASSERT_EQ(vec[3], S(4));
    ASSERT_EQ(vec[5], S(6));
    ASSERT_THROW(vec[6], std::runtime_error);

    ASSERT_EQ(vec.front(), S(1));
    ASSERT_EQ(vec.back(), S(6));
    ASSERT_FALSE(vec.contains(S(8)));
    ASSERT_TRUE(vec.contains(S(3)));
    ASSERT_TRUE(vec.contains_if(even_s));
    ASSERT_FALSE(vec.contains_if(gt100_s));
    ASSERT_EQ(vec.data(), vec.begin());
  }

  {
    const StaticVector<S, 2> empty{};

    ASSERT_THROW(empty[0], std::runtime_error);
    ASSERT_THROW(empty[1], std::runtime_error);
    ASSERT_THROW(empty.front(), std::runtime_error);
    ASSERT_THROW(empty.back(), std::runtime_error);
    ASSERT_FALSE(empty.contains(S(10)));
    ASSERT_FALSE(empty.contains_if(even_s));
    ASSERT_EQ(empty.data(), empty.begin());

    auto const vec = SV_S<10>(1, 2, 3, 4, 5, 6);

    ASSERT_EQ(vec[0], S(1));
    ASSERT_EQ(vec[3], S(4));
    ASSERT_EQ(vec[5], S(6));
    ASSERT_THROW(vec[6], std::runtime_error);

    ASSERT_EQ(vec.front(), S(1));
    ASSERT_EQ(vec.back(), S(6));
    ASSERT_FALSE(vec.contains(S(8)));
    ASSERT_TRUE(vec.contains(S(3)));
    ASSERT_TRUE(vec.contains_if(even_s));
    ASSERT_FALSE(vec.contains_if(gt100_s));
    ASSERT_EQ(vec.data(), vec.begin());
  }
}

// StaticVector - S_capacity
TEST(StaticVector, S_capacity)
{
  loguru::init_for_test("S_capacity");

  StaticVector<S, 2> empty{};

  ASSERT_TRUE(empty.empty());
  ASSERT_FALSE(empty.any());
  ASSERT_FALSE(empty.full());
  ASSERT_EQ(empty.size(), 0);

  auto vec = SV_S<2>(1);
  ASSERT_FALSE(vec.empty());
  ASSERT_TRUE(vec.any());
  ASSERT_FALSE(vec.full());
  ASSERT_EQ(vec.size(), 1);

  auto vec2 = SV_S<2>(1, 2);
  ASSERT_FALSE(vec2.empty());
  ASSERT_TRUE(vec2.any());
  ASSERT_TRUE(vec2.full());
  ASSERT_EQ(vec2.size(), 2);
}

// StaticVector - S_modifiers_insert
TEST(StaticVector, S_modifiers_insert)
{
  loguru::init_for_test("S_modifiers_insert");

  // push_back
  {
    StaticVector<S, 3> vec{};
    check(vec.push_back(S(1)), V_S(1));
    check(vec.push_back(S(2)), V_S(1, 2));
    check(vec.push_back(S(3)), V_S(1, 2, 3));
    ASSERT_THROW(vec.push_back(S(4)), std::runtime_error);
    check(vec, V_S(1, 2, 3));
  }

  // push_front
  {
    StaticVector<S, 3> vec{};
    check(vec.push_front(S(1)), V_S(1));
    check(vec.push_front(S(2)), V_S(2, 1));
    check(vec.push_front(S(3)), V_S(3, 2, 1));
    ASSERT_THROW(vec.push_front(S(4)), std::runtime_error);
    check(vec, V_S(3,2,1));
  }

  // push_at
  {
    auto vec = SV_S<7>(1, 2, 3, 4, 5, 6);
    check(vec.push_at(2, S(10)), V_S(1,2,10,3,4,5,6));
    ASSERT_THROW(vec.push_at(2, S(20)), std::runtime_error);
    check(vec, V_S(1,2,10,3,4,5,6));

    vec = SV_S<7>(1);
    check(vec.push_at(1, S(10)), V_S(1,10)); // at the end
  }

  // insert (value)
  {
    auto vec = SV_S<7>(1, 2, 3, 4, 5, 6);
    ASSERT_EQ(vec.begin() + 2, vec.insert(vec.begin() + 2, S(10)));
    check(vec, V_S(1,2,10,3,4,5,6));
    ASSERT_THROW(vec.insert(vec.begin() + 2, S(20)), std::runtime_error);
    check(vec, V_S(1,2,10,3,4,5,6));

    vec = SV_S<7>(1);
    ASSERT_EQ(vec.begin() + 1, vec.insert(vec.end(), S(10)));
    check(vec, V_S(1, 10));
  }

  // insert (value/count)
  {
    auto vec = SV_S<10>(1, 2, 3, 4, 5, 6);
    ASSERT_EQ(vec.begin() + 2, vec.insert(vec.begin() + 2, 2, S(10)));
    check(vec, V_S(1,2,10,10,3,4,5,6));
    ASSERT_THROW(vec.insert(vec.begin() + 2, 3, S(20)), std::runtime_error);
    check(vec, V_S(1,2,10,10,3,4,5,6));

    vec = SV_S<7>(1);
    ASSERT_EQ(vec.begin() + 1, vec.insert(vec.end(), 2, S(10)));
    check(vec, V_S(1, 10, 10));
  }

  // insert (iterator)
  {
    auto vec = SV_S<10>(1, 2, 3, 4, 5, 6);
    auto iv = V_S(10, 11, 12);
    ASSERT_EQ(vec.begin() + 2, vec.insert(vec.begin() + 2, iv.begin(), iv.begin() + 2));
    check(vec, V_S(1,2,10,11,3,4,5,6));
    ASSERT_THROW(vec.insert(vec.begin() + 2, iv.begin(), iv.end()), std::runtime_error);
    check(vec, V_S(1,2,10,11,3,4,5,6));

    vec = SV_S<7>(1);
    ASSERT_EQ(vec.begin() + 1, vec.insert(vec.end(), iv.begin(), iv.end()));
    check(vec, V_S(1, 10, 11, 12));
  }

  // insert (initializer list)
  {
    auto vec = SV_S<10>(1, 2, 3, 4, 5, 6);
    auto iv = V_S(10, 11, 12);
    ASSERT_EQ(vec.begin() + 2, vec.insert(vec.begin() + 2, {S(10), S(11)}));
    check(vec, V_S(1,2,10,11,3,4,5,6));
    ASSERT_THROW(vec.insert(vec.begin() + 2, {S(10), S(11), S(12)}), std::runtime_error);
    check(vec, V_S(1,2,10,11,3,4,5,6));

    vec = SV_S<7>(1);
    ASSERT_EQ(vec.begin() + 1, vec.insert(vec.end(), {S(10), S(11), S(12)}));
    check(vec, V_S(1, 10, 11, 12));
  }

}

// StaticVector - S_modifiers_remove
TEST(StaticVector, S_modifiers_remove)
{
  loguru::init_for_test("S_modifiers_remove");

  // pop (empty)
  {
    StaticVector<S, 3> empty{};
    ASSERT_THROW(empty.pop_back(), std::runtime_error);
    ASSERT_THROW(empty.pop_at(2), std::runtime_error);
    ASSERT_THROW(empty.pop_front(), std::runtime_error);
  }

  // pop (not empty)
  {
    auto vec = SV_S<10>(1, 2, 3, 4, 5, 6);

    check(vec.pop_back(), V_S(1,2,3,4,5));
    check(vec.pop_front(), V_S(2,3,4,5));
    check(vec.pop_at(2), V_S(2,3,5));
    ASSERT_THROW(vec.pop_at(3), std::runtime_error);
    check(vec, V_S(2,3,5));
  }

  // drop (empty)
  {
    StaticVector<S, 3> empty{};
    ASSERT_THROW(empty.drop_back(), std::runtime_error);
    ASSERT_THROW(empty.drop_at(2), std::runtime_error);
    ASSERT_THROW(empty.drop_front(), std::runtime_error);
    ASSERT_EQ(0, empty.drop(S(1)));
    ASSERT_EQ(0, empty.drop_if(even_s));
    auto v = V_S();
    ASSERT_EQ(0, empty.drop_if(std::back_inserter(v), even_s));
  }

  // drop (not empty)
  {
    auto vec = SV_S<10>(1, 2, 3, 4, 5, 6);

    ASSERT_EQ(S(6), vec.drop_back());
    check(vec, V_S(1,2,3,4,5));
    ASSERT_EQ(S(1), vec.drop_front());
    check(vec, V_S(2,3,4,5));
    ASSERT_EQ(S(4), vec.drop_at(2));
    check(vec, V_S(2,3,5));
    ASSERT_THROW(vec.drop_at(3), std::runtime_error);
    check(vec, V_S(2,3,5));

    vec = SV_S<10>(1, 2, 3, 4, 5, 6, 3);
    ASSERT_EQ(2, vec.drop(S(3)));
    check(vec, V_S(1,2,4,5,6));

    vec = SV_S<10>(1, 2, 3, 4, 5, 6);
    ASSERT_EQ(3, vec.drop_if(even_s));
    check(vec, V_S(1,3,5));
    ASSERT_EQ(0, vec.drop_if(gt100_s));

    vec = SV_S<10>(1, 2, 3, 4, 5, 6);
    auto v = V_S();
    ASSERT_EQ(3, vec.drop_if(std::back_inserter(v), even_s));
    check(vec, V_S(1,3,5));
    ASSERT_EQ(v, V_S(2,4,6));
  }

  // erase
  {
    StaticVector<S, 3> empty{};
    ASSERT_THROW(empty.erase(empty.begin()), std::runtime_error);
    ASSERT_THROW(empty.erase(empty.begin() + 1), std::runtime_error);


    auto vec = SV_S<10>(1, 2, 3, 4, 5, 6);

    ASSERT_EQ(vec.begin() + 2, vec.erase(vec.begin() + 2)); // erase (1)
    check(vec, V_S(1,2,4,5,6));
    ASSERT_THROW(vec.erase(vec.end()), std::runtime_error);

    ASSERT_EQ(vec.rbegin() + 1, vec.erase(vec.rbegin() + 1)); // erase (from the back)
    check(vec, V_S(1,2,4,6));
    ASSERT_THROW(vec.erase(vec.rend()), std::runtime_error);

    vec = SV_S<10>(1, 2, 3, 4, 5, 6);
    ASSERT_EQ(vec.begin() + 2, vec.erase(vec.begin() + 2, vec.begin() + 4)); // erase (multiple)
    check(vec, V_S(1,2,5,6));

    ASSERT_THROW(vec.erase(vec.begin() + 2, vec.end() + 1), std::runtime_error); // last too big
    ASSERT_THROW(vec.erase(vec.end(), vec.end() + 1), std::runtime_error); // first too big
    ASSERT_THROW(vec.erase(vec.begin() + 2, vec.begin()), std::runtime_error); // first > last
  }


}

TEST(StaticVector, Int)
{
  StaticVector<int, 10> v{{1,2,3,4,5,6}};
  ASSERT_EQ(std::vector<int>({1,2,3,4,5,6}), toVector(v));

  ASSERT_EQ(6, v.size());
  ASSERT_FALSE(v.full());
  ASSERT_FALSE(v.empty());

  StaticVector<int, 10> r{};
  ASSERT_EQ(std::vector<int>(), toVector(r));

  v.drop_if(std::back_inserter(r), [](auto i) { return i % 2 == 0; });

  ASSERT_EQ(std::vector<int>({1,3,5}), toVector(v));
  ASSERT_EQ(std::vector<int>({2,4,6}), toVector(r));

  ASSERT_EQ(5, v.drop_back());
  ASSERT_EQ(std::vector<int>({1,3}), toVector(v));

  ASSERT_EQ(1, v.drop_front());
  ASSERT_EQ(std::vector<int>({3}), toVector(v));


  //std::back_insert_iterator;

}

}