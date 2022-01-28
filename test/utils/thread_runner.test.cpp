/**
 * Copyright Claus Beckenbauer 2004 - 2022.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
 */

#include "utils/thread_runner.hpp"

#include <boost/test/unit_test.hpp>

namespace utils
{

BOOST_AUTO_TEST_SUITE(suite_thread_runner)

struct Fixture
{
};

BOOST_FIXTURE_TEST_CASE(test_run_stop, Fixture)
{
    auto tr = thread_runner{"test_runner", []() {}, []() {}};
    BOOST_CHECK(tr.run());
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace utils
