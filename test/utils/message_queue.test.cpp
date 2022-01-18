/**
 * Copyright Claus Beckenbauer 2004 - 2022.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */

#include "utils/message_queue.hpp"

#include <boost/test/unit_test.hpp>

#include "io.hpp"

namespace utils
{

BOOST_AUTO_TEST_SUITE(suite_message_queue)

struct Fixture
{
    using msg_type = std::vector<char>;
};

BOOST_FIXTURE_TEST_CASE(test_enqueue, Fixture)
{
    auto q = utils::message_queue<msg_type, 3>{};
    BOOST_TEST(q.enqueue({'1'}));
    BOOST_TEST(q.enqueue({'2'}));
    BOOST_TEST(q.enqueue({'3'}));
    BOOST_TEST(!q.enqueue({'4'}));
}

BOOST_FIXTURE_TEST_CASE(test_dequeue, Fixture)
{
    auto q = utils::message_queue<msg_type, 3>{};
    BOOST_TEST_REQUIRE(q.enqueue({'5'}));
    BOOST_TEST_REQUIRE(q.enqueue({'6'}));
    BOOST_TEST_REQUIRE(q.enqueue({'7'}));
    BOOST_TEST_REQUIRE(!q.enqueue({'8'}));

    {
        auto msg = q.dequeue();
        BOOST_CHECK_EQUAL(msg[0], '5');
        BOOST_TEST_REQUIRE(q.enqueue({'8'}));
        BOOST_TEST_REQUIRE(!q.enqueue({'9'}));
    }
    for (auto c : {'6', '7', '8'})
    {
        auto msg = q.dequeue();
        BOOST_CHECK_EQUAL(msg[0], c);
    }
}

BOOST_FIXTURE_TEST_CASE(test_watermark, Fixture)
{
    BOOST_CHECK_NO_THROW((utils::message_queue<msg_type, 3>{{0, []() {}}, {1, []() {}}}));
    BOOST_CHECK_NO_THROW((utils::message_queue<msg_type, 3>{{2, []() {}}, {3, []() {}}}));

    BOOST_CHECK_THROW((utils::message_queue<msg_type, 3>{{0, []() {}}, {4, []() {}}}), std::invalid_argument);
    BOOST_CHECK_THROW((utils::message_queue<msg_type, 3>{{1, []() {}}, {0, []() {}}}), std::invalid_argument);

    auto low_mark_reached  = false;
    auto high_mark_reached = false;
    auto q                 = utils::message_queue<msg_type, 3>{{0,
                                                [&low_mark_reached]()
                                                {
                                                    BOOST_TEST_MESSAGE("low watermark reached");
                                                    low_mark_reached = true;
                                                }},
                                               {3, [&high_mark_reached]()
                                                {
                                                    BOOST_TEST_MESSAGE("high watermark reached");
                                                    high_mark_reached = true;
                                                }}};

    BOOST_TEST(q.enqueue({'1'}));
    BOOST_CHECK_EQUAL(low_mark_reached, false);
    BOOST_CHECK_EQUAL(high_mark_reached, false);

    BOOST_TEST(q.enqueue({'2'}));
    BOOST_CHECK_EQUAL(low_mark_reached, false);
    BOOST_CHECK_EQUAL(high_mark_reached, false);

    BOOST_TEST(q.enqueue({'3'}));
    BOOST_CHECK_EQUAL(low_mark_reached, false);
    BOOST_CHECK_EQUAL(high_mark_reached, true);

    BOOST_TEST(!q.enqueue({'4'}));
    BOOST_CHECK_EQUAL(low_mark_reached, false);
    BOOST_CHECK_EQUAL(high_mark_reached, true);

    for (auto c : {'1', '2', '3'}) q.dequeue();
    BOOST_CHECK_EQUAL(low_mark_reached, false);
    BOOST_CHECK_EQUAL(high_mark_reached, true);

    BOOST_TEST(q.enqueue({'4'}));
    BOOST_CHECK_EQUAL(low_mark_reached, true);
    BOOST_CHECK_EQUAL(high_mark_reached, true);
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace utils