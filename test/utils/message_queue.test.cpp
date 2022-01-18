/**
 * Copyright Claus Beckenbauer 2004 - 2022.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */

#include "utils/message_queue.hpp"

#include <boost/test/unit_test.hpp>
#include <span>
namespace utils
{

BOOST_AUTO_TEST_SUITE(suite_message_queue)

struct Fixture
{
    using msg_type = std::array<char, 1>;
    auto make_msg(char c) { return std::make_unique<msg_type>(msg_type{c}); }
};

BOOST_FIXTURE_TEST_CASE(test_enqueue, Fixture)
{
    auto q = utils::message_queue<msg_type, 3>{};
    BOOST_TEST(q.enqueue(make_msg('1')));
    BOOST_TEST(q.enqueue(make_msg('2')));
    BOOST_TEST(q.enqueue(make_msg('3')));
    BOOST_TEST(!q.enqueue(make_msg('4')));
}

BOOST_FIXTURE_TEST_CASE(test_dequeue, Fixture)
{
    auto q = utils::message_queue<msg_type, 3>{};
    BOOST_TEST_REQUIRE(q.enqueue(make_msg('5')));
    BOOST_TEST_REQUIRE(q.enqueue(make_msg('6')));
    BOOST_TEST_REQUIRE(q.enqueue(make_msg('7')));
    BOOST_TEST_REQUIRE(!q.enqueue(make_msg('8')));

    {
        auto msg = q.dequeue();
        BOOST_CHECK_EQUAL((*msg)[0], '5');
        BOOST_TEST_REQUIRE(q.enqueue(make_msg('8')));
        BOOST_TEST_REQUIRE(!q.enqueue(make_msg('9')));
    }
    for (auto c : {'6', '7', '8'})
    {
        auto msg = q.dequeue();
        BOOST_CHECK_EQUAL((*msg)[0], c);
    }
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace utils