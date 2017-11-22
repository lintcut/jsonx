/*
* Copyright (c) 2017 Xiang Ye
* All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/

#include <Windows.h>
#include <jsonx/jsonx.hpp>

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(JsonValueTest)

BOOST_AUTO_TEST_CASE(TestValueNull)
{
    JSONX::Value val(JSONX::JsonNull);
    BOOST_CHECK(val.isNull());
}

BOOST_AUTO_TEST_CASE(TestValueBoolean)
{
    JSONX::Value val1(JSONX::JsonBoolean);
    BOOST_CHECK(val1.isBoolean());
    BOOST_CHECK(!val1.getBoolean());

    JSONX::Value val2(true);
    BOOST_CHECK(val2.isBoolean());
    BOOST_CHECK(val2.getBoolean());

    JSONX::Value val3(false);
    BOOST_CHECK(val3.isBoolean());
    BOOST_CHECK(!val3.getBoolean());
}

BOOST_AUTO_TEST_CASE(TestValueNumber)
{
    JSONX::Value val1(JSONX::JsonNumber);
    BOOST_CHECK(val1.isNumber());
    BOOST_CHECK_EQUAL(0, val1.getInt32());

    JSONX::Value val2(-110);
    BOOST_CHECK(val2.isNumber());
    BOOST_CHECK(val2.isIntegerNumber());
    BOOST_CHECK(val2.isSignedNumber());
    BOOST_CHECK_EQUAL(-110, val2.getInt32());

    JSONX::Value val3(1000);
    BOOST_CHECK(val3.isNumber());
    BOOST_CHECK(val3.isIntegerNumber());
    BOOST_CHECK(!val3.isSignedNumber());
    BOOST_CHECK_EQUAL(1000, val3.getInt32());

    JSONX::Value val4(0x7FFFFFFFFFFF);
    BOOST_CHECK(val4.isNumber());
    BOOST_CHECK(val4.isIntegerNumber());
    BOOST_CHECK(!val4.isSignedNumber());
    BOOST_CHECK_EQUAL(0x7FFFFFFFFFFF, val4.getInt64());

    JSONX::Value val5(-1000.329);
    BOOST_CHECK(val5.isNumber());
    BOOST_CHECK(val5.isDecimalNumber());
    BOOST_CHECK(val5.isSignedNumber());
    BOOST_CHECK(std::abs(val5.getDecimal() + 1000.329) <= 0.000001);
}

BOOST_AUTO_TEST_CASE(TestValueString)
{
    JSONX::Value val1(JSONX::JsonString);
    BOOST_CHECK(val1.isString());
    BOOST_CHECK("" == val1.getString());

    JSONX::Value val2("Hello\\u0020World", true);
    BOOST_CHECK(val2.isString());
    BOOST_CHECK("Hello World" == val2.getString());

    JSONX::Value val3("Hello\"World\"\r\n", false);
    BOOST_CHECK(val3.isString());
    BOOST_CHECK("Hello\"World\"\r\n" == val3.getString());
}

BOOST_AUTO_TEST_CASE(TestValueObject)
{
    JSONX::Value val(JSONX::JsonObject);
    BOOST_CHECK(val.isObject());

    val.set("name", "Jogn Tyler");
    val.set("age", 27);
    val.set("address", "1129 S. Grant Street");
    val.set("height", 5.9);
    val.set("weight", 86.43);
    JSONX::Value valObj = val.set("phone", JSONX::Value(JSONX::JsonObject));
    valObj.set("home", std::string("650-621-3587"));
    valObj.set("mobile", std::string("650-112-9989"));
    JSONX::Value valArray = val.set("otherName", JSONX::Value(JSONX::JsonArray));
    valArray.push_back(std::string("Johnny"));
    valArray.push_back(std::string("JohnT"));

    const std::string& s = val.serialize();
}

BOOST_AUTO_TEST_SUITE_END()
