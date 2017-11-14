#include <Windows.h>
#include <jsonx/jsonx.hpp>

#define BOOST_TEST_MODULE JSONX
#include <boost/test/unit_test.hpp>


BOOST_AUTO_TEST_SUITE(JsonTypeTest)


BOOST_AUTO_TEST_CASE(TestJsonNull)
{
    BOOST_CHECK_EQUAL(1, 1);
}

BOOST_AUTO_TEST_SUITE_END()
