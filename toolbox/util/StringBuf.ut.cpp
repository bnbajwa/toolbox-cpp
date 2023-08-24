// The Reactive C++ Toolbox.
// Copyright (C) 2013-2019 Swirly Cloud Limited
// Copyright (C) 2021 Reactive Markets Limited
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "StringBuf.hpp"

#include <boost/test/unit_test.hpp>

using namespace std;
using namespace toolbox;

BOOST_AUTO_TEST_SUITE(StringBufSuite)

BOOST_AUTO_TEST_CASE(StringBufEmptyCase)
{
    StringBuf<3> sb;
    BOOST_CHECK(sb.empty());
    BOOST_CHECK_EQUAL(sb.size(), 0U);
    BOOST_CHECK_EQUAL(sb, ""sv);
    auto res = sb == StringBuf<3>{""sv};
    BOOST_CHECK(res);
}

BOOST_AUTO_TEST_CASE(StringBufNonEmptyCase)
{
    StringBuf<3> sb{"Foo"sv};
    BOOST_CHECK(!sb.empty());
    BOOST_CHECK_EQUAL(sb.size(), 3U);
    BOOST_CHECK_EQUAL(sb, "Foo"sv);
    auto res = sb == StringBuf<3>{"Foo"sv};
    BOOST_CHECK(res);
}

BOOST_AUTO_TEST_CASE(StringBufUpperBoundCase)
{
    StringBuf<3> sb{"Foox"sv};
    BOOST_CHECK_EQUAL(sb.size(), 3U);
    BOOST_CHECK_EQUAL(sb, "Foo"sv);

    sb = "Barx"sv;
    BOOST_CHECK_EQUAL(sb.size(), 3U);
    BOOST_CHECK_EQUAL(sb, "Bar"sv);
}

BOOST_AUTO_TEST_CASE(StringBufClearCase)
{
    StringBuf<3> sb{"Foo"sv};
    sb.clear();
    BOOST_CHECK(sb.empty());
}

BOOST_AUTO_TEST_CASE(StringBufAssignCase)
{
    StringBuf<3> sb;
    sb = "Foo"sv;
    BOOST_CHECK_EQUAL(sb.size(), 3U);
    BOOST_CHECK_EQUAL(sb, "Foo"sv);

    sb.clear();
    sb = "Foox"sv;
    BOOST_CHECK_EQUAL(sb.size(), 3U);
    BOOST_CHECK_EQUAL(sb, "Foo"sv);
}

BOOST_AUTO_TEST_CASE(StringBufAppendCase)
{
    StringBuf<6> sb;
    sb = "Foo"sv;
    sb += "Bar"sv;
    BOOST_CHECK_EQUAL(sb.size(), 6U);
    BOOST_CHECK_EQUAL(sb, "FooBar"sv);

    sb = "Foo"sv;
    sb += "Barx"sv;
    BOOST_CHECK_EQUAL(sb.size(), 6U);
    BOOST_CHECK_EQUAL(sb, "FooBar"sv);
}

BOOST_AUTO_TEST_CASE(StringBufLexicalCompare)
{
    StringBuf<6> sb;
    sb = "b"sv;
    BOOST_CHECK_GT(sb, "a"sv);
    BOOST_CHECK_EQUAL(sb, "b"sv);
    BOOST_CHECK_LT(sb, "c"sv);
    BOOST_CHECK_NE(sb, "zz"sv);
}

BOOST_AUTO_TEST_SUITE_END()
