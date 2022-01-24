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

#include <pongasoft/common/stl.h>
#include <gtest/gtest.h>
#include <vector>
#include <logging.h>

namespace pongasoft::common::Test {

namespace test_stl {

struct S {
  int x{};

  bool operator==(S const &rhs) const
  {
    return x == rhs.x;
  }

  bool operator!=(S const &rhs) const
  {
    return !(rhs == *this);
  }
};

}

using S = test_stl::S;

TEST(stl, maybe_value)
{
  ASSERT_EQ(typeid(std::optional<S>), typeid(stl::maybe_value_t<S>));
  ASSERT_EQ(typeid(S*), typeid(stl::maybe_value_t<S*>));

  ASSERT_EQ(typeid(std::optional<S>), typeid(decltype(stl::first_or_null(std::vector<S>()))));
  ASSERT_EQ(typeid(S*), typeid(decltype(stl::first_or_null(std::vector<S*>()))));
}

TEST(stl, remove_if)
{
  // vector
  {
    std::vector<int> v{1, 2, 3, 4, 5};
    std::vector<int> r{};

    ASSERT_EQ(v.end(), stl::remove_if(v.begin(), v.end(), std::back_inserter(r), [](auto i) { return i > 100; }));
    ASSERT_EQ(5, v.size());
    ASSERT_TRUE(r.empty());

    auto iter = stl::remove_if(v.begin(), v.end(), std::back_inserter(r), [](auto i) { return i % 2 == 0; });
    ASSERT_EQ(iter, v.begin() + 3);
    ASSERT_EQ(5, v.size()); // size has NOT changed!
    ASSERT_EQ(std::vector<int>({1, 3, 5, 4, 5}), v);
    ASSERT_EQ(std::vector<int>({2, 4}), r);
  }

  // array
  {
    std::vector<int> r{};
    int a[] = {1,2,3,4,5};
    ASSERT_EQ(std::end(a), stl::remove_if(std::begin(a), std::end(a), std::back_inserter(r), [](auto i) { return i > 100; }));
    ASSERT_TRUE(r.empty());

    auto iter = stl::remove_if(std::begin(a), std::end(a), std::back_inserter(r), [](auto i) { return i % 2 == 0; });
    ASSERT_EQ(iter, std::begin(a) + 3);
    ASSERT_EQ(std::vector<int>({1, 3, 5, 4, 5}), std::vector<int>(std::begin(a), std::end(a)));
    ASSERT_EQ(std::vector<int>({2, 4}), r);
  }
}

TEST(stl, contains_if)
{
  // vector<int>
  {
    std::vector<int> v{1, 2, 3, 4, 5};
    const std::vector<int> vc{1, 2, 3, 4, 5};

    ASSERT_FALSE(stl::contains_if(v, [](auto i) { return i > 100; }));
    ASSERT_TRUE(stl::contains_if(v, [](auto i) { return i % 2 == 0; }));

    ASSERT_FALSE(stl::contains_if(vc, [](auto i) { return i > 100; }));
    ASSERT_TRUE(stl::contains_if(vc, [](auto i) { return i % 2 == 0; }));
  }

  // vector<S>
  {
    std::vector<S> v{S{1}, S{2}, S{3}, S{4}, S{5}};
    const std::vector<S> vc{S{1}, S{2}, S{3}, S{4}, S{5}};


    ASSERT_FALSE(stl::contains_if(v, [](auto i) { return i.x > 100; }));
    ASSERT_TRUE(stl::contains_if(v, [](auto i) { return i.x % 2 == 0; }));

    ASSERT_FALSE(stl::contains_if(vc, [](auto i) { return i.x > 100; }));
    ASSERT_TRUE(stl::contains_if(vc, [](auto i) { return i.x % 2 == 0; }));
  }

  // array
  {
    int a[] = {1,2,3,4,5};
    const int ac[] = {1,2,3,4,5};

    ASSERT_FALSE(stl::contains_if(a, [](auto i) { return i > 100; }));
    ASSERT_TRUE(stl::contains_if(a, [](auto i) { return i % 2 == 0; }));

    ASSERT_FALSE(stl::contains_if(ac, [](auto i) { return i > 100; }));
    ASSERT_TRUE(stl::contains_if(ac, [](auto i) { return i % 2 == 0; }));
  }
}

TEST(stl, first_or_null)
{
  // vector<int>
  {
    std::vector<int> v{1, 2, 3, 4, 5};
    const std::vector<int> vc{1, 2, 3, 4, 5};

    ASSERT_EQ(std::nullopt, stl::first_or_null(std::vector<int>{}));

    ASSERT_EQ(std::optional<int>(1), stl::first_or_null(v));

    ASSERT_EQ(std::optional<int>(1), stl::first_or_null(vc));
  }

  // vector<int*>
  {
    std::vector<int> s{1, 2, 3, 4, 5};

    std::vector<int *> v{&s[0], &s[1], &s[2], &s[3], &s[4]};
    const std::vector<int *> vc{&s[0], &s[1], &s[2], &s[3], &s[4]};

    ASSERT_EQ(nullptr, stl::first_or_null(std::vector<int *>{}));

    ASSERT_EQ(&s[0], stl::first_or_null(v));

    ASSERT_EQ(&s[0], stl::first_or_null(vc));
  }

  // vector<S>
  {
    std::vector<S> v{S{1}, S{2}, S{3}, S{4}, S{5}};
    const std::vector<S> vc{S{1}, S{2}, S{3}, S{4}, S{5}};

    {
      auto res = stl::first_or_null(v);
      ASSERT_EQ(std::optional<S>(S{1}), res);
      ASSERT_EQ(S{1}, *res);
      res->x = 3;
      ASSERT_EQ(S{3}, *res); // ensures that we got a copy
      ASSERT_EQ(std::vector<S>({S{1}, S{2}, S{3}, S{4}, S{5}}), v); // and modifying it does not affect v!
    }

    {
      auto res = stl::first_or_null(vc);
      ASSERT_EQ(std::optional<S>(S{1}), res);
      ASSERT_EQ(S{1}, *res);
      res->x = 3;
      ASSERT_EQ(S{3}, *res); // ensures that we got a copy
      ASSERT_EQ(std::vector<S>({S{1}, S{2}, S{3}, S{4}, S{5}}), vc); // and modifying it does not affect v!
    }
  }

  // array
  {
    int a[] = {1,2,3,4,5};
//    int empty[0] = {}; 0 lentgth array are not allowed in C++
    const int ac[] = {1,2,3,4,5};

    ASSERT_EQ(std::optional<int>(1), stl::first_or_null(a));
    ASSERT_EQ(std::optional<int>(1), stl::first_or_null(ac));
  }
}

TEST(stl, find_if_or_null)
{
  // vector<int>
  {
    std::vector<int> v{1, 2, 3, 4, 5};
    const std::vector<int> vc{1, 2, 3, 4, 5};

    ASSERT_EQ(std::nullopt, stl::find_if_or_null(v, [](auto i) { return i > 100; }));
    ASSERT_EQ(std::optional<int>(2), stl::find_if_or_null(v, [](auto i) { return i % 2 == 0; }));

    ASSERT_EQ(std::nullopt, stl::find_if_or_null(vc, [](auto i) { return i > 100; }));
    ASSERT_EQ(std::optional<int>(2), stl::find_if_or_null(vc, [](auto i) { return i % 2 == 0; }));
  }

  // vector<int *>
  {
    std::vector<int> s{1, 2, 3, 4, 5};

    std::vector<int *> v{&s[0], &s[1], &s[2], &s[3], &s[4]};
    const std::vector<int *> vc{&s[0], &s[1], &s[2], &s[3], &s[4]};

    ASSERT_EQ(nullptr, stl::find_if_or_null(v, [](auto i) { return *i > 100; }));
    ASSERT_EQ(&s[1], stl::find_if_or_null(v, [](auto i) { return *i % 2 == 0; }));

    ASSERT_EQ(nullptr, stl::find_if_or_null(vc, [](auto i) { return *i > 100; }));
    ASSERT_EQ(&s[1], stl::find_if_or_null(vc, [](auto i) { return *i % 2 == 0; }));
  }

  // vector<S>
  {
    std::vector<S> v{S{1}, S{2}, S{3}, S{4}, S{5}};
    const std::vector<S> vc{S{1}, S{2}, S{3}, S{4}, S{5}};

    ASSERT_EQ(std::nullopt, stl::find_if_or_null(v, [](auto i) { return i.x > 100; }));

    {
      auto res = stl::find_if_or_null(v, [](auto i) { return i.x % 2 == 0; });
      ASSERT_EQ(std::optional<S>(S{2}), res);
      ASSERT_EQ(S{2}, *res);
      res->x = 3;
      ASSERT_EQ(S{3}, *res); // ensures that we got a copy
      ASSERT_EQ(std::vector<S>({S{1}, S{2}, S{3}, S{4}, S{5}}), v); // and modifying it does not affect v!
    }

    ASSERT_EQ(std::nullopt, stl::find_if_or_null(vc, [](auto i) { return i.x > 100; }));

    {
      auto res = stl::find_if_or_null(vc, [](auto i) { return i.x % 2 == 0; });
      ASSERT_EQ(std::optional<S>(S{2}), res);
      ASSERT_EQ(S{2}, *res);
      res->x = 3;
      ASSERT_EQ(S{3}, *res); // ensures that we got a copy
      ASSERT_EQ(std::vector<S>({S{1}, S{2}, S{3}, S{4}, S{5}}), vc); // and modifying it does not affect v!
    }
  }

  // array
  {
    int a[] = {1,2,3,4,5};
    const int ac[] = {1,2,3,4,5};

    ASSERT_EQ(std::nullopt, stl::find_if_or_null(a, [](auto i) { return i > 100; }));
    ASSERT_EQ(std::optional<int>(2), stl::find_if_or_null(a, [](auto i) { return i % 2 == 0; }));

    ASSERT_EQ(std::nullopt, stl::find_if_or_null(ac, [](auto i) { return i > 100; }));
    ASSERT_EQ(std::optional<int>(2), stl::find_if_or_null(ac, [](auto i) { return i % 2 == 0; }));
  }
}

TEST(stl, for_each)
{
  // vector<int>
  {
    std::vector<int> v{1, 2, 3, 4, 5};
    const std::vector<int> vc{1, 2, 3, 4, 5};

    stl::for_each(v, [](auto &i) { i *=2; });
    ASSERT_EQ(std::vector<int>({2,4,6,8,10}), v);

    std::vector<int> v2{};
    stl::for_each(vc, [&v2](auto i) { v2.emplace_back(i * 2); });
    ASSERT_EQ(std::vector<int>({1,2,3,4,5}), vc);
    ASSERT_EQ(std::vector<int>({2,4,6,8,10}), v2);
  }

  // vector<S>
  {
    std::vector<S> v{S{1}, S{2}, S{3}, S{4}, S{5}};
    const std::vector<S> vc{S{1}, S{2}, S{3}, S{4}, S{5}};


    stl::for_each(v, [](auto &i) { i.x *=2; });
    ASSERT_EQ(std::vector<S>({S{2}, S{4}, S{6}, S{8}, S{10}}), v);

    std::vector<S> v2{};
    stl::for_each(vc, [&v2](auto i) { v2.emplace_back(S({i.x * 2})); });
    ASSERT_EQ(std::vector<S>({S{1}, S{2}, S{3}, S{4}, S{5}}), vc);
    ASSERT_EQ(std::vector<S>({S{2}, S{4}, S{6}, S{8}, S{10}}), v2);
  }

  // array
  {
    int a[] = {1,2,3,4,5};
    const int ac[] = {1,2,3,4,5};

    stl::for_each(a, [](auto &i) { i *=2; });
    ASSERT_EQ(std::vector<int>({2, 4, 6, 8, 10}), std::vector<int>(std::begin(a), std::end(a)));

    std::vector<int> v2{};
    stl::for_each(ac, [&v2](auto i) { v2.emplace_back(i * 2); });
    ASSERT_EQ(std::vector<int>({2,4,6,8,10}), v2);
    ASSERT_EQ(std::vector<int>({1,2,3,4,5}), std::vector<int>(std::begin(ac), std::end(ac)));

  }
}

}