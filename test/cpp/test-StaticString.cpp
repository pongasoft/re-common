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

#include <StaticString.h>
#include <gtest/gtest.h>

namespace Test {

template<size_t N>
auto compare(StaticString<N> const &s1, std::string const &s2)
{
  return std::string(s1).compare(s2);
}

TEST(StaticString, constructor)
{
  ASSERT_EQ(0, compare(StaticString<10>("abc"), "abc"));
  ASSERT_EQ(0, compare(StaticString<10>(""), ""));
  ASSERT_EQ(0, compare(StaticString<1>(""), ""));

  // note that StaticString<0> cannot be instantiated/compiled

  // char[3] -> ['a', 'b', '\0'] => "ab"
  ASSERT_EQ(0, compare(StaticString<3>("abc"), "ab"));
}

TEST(StaticString, compare)
{
  ASSERT_EQ(0, StaticString<10>("abc").compare(StaticString<10>("abc")));
  ASSERT_EQ(0, StaticString<10>("abc").compare(StaticString<5>("abc")));
  ASSERT_TRUE(StaticString<10>("abc").compare(StaticString<10>("abd")) < 0);
  ASSERT_TRUE(StaticString<10>("abc").compare(StaticString<5>("abd")) < 0);
  ASSERT_TRUE(StaticString<10>("abd").compare(StaticString<10>("abc")) > 0);
  ASSERT_TRUE(StaticString<10>("abd").compare(StaticString<5>("abc")) > 0);

  ASSERT_TRUE(StaticString<10>("abc").compare(StaticString<10>("abcd")) < 0);
  ASSERT_TRUE(StaticString<10>("abc").compare(StaticString<5>("abcd")) < 0);
}

TEST(StaticString, LessThan)
{
  ASSERT_TRUE(StaticString<10>("abc") < StaticString<10>("abd"));
  ASSERT_TRUE(StaticString<10>("abc") < StaticString<5>("abd"));
  ASSERT_TRUE(StaticString<10>("abc") < StaticString<10>("abcd"));
  ASSERT_TRUE(StaticString<10>("abc") < StaticString<5>("abcd"));
}

}