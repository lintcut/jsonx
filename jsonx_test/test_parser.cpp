#include <Windows.h>
#include <jsonx/jsonx.hpp>

//#define BOOST_TEST_MODULE JSONX
#include <boost/test/unit_test.hpp>


BOOST_AUTO_TEST_SUITE(JsonParserTest)

BOOST_AUTO_TEST_CASE(CheckNull)
{
    std::istringstream ss1(std::string("null"));
    JSONX::IMPLEMENT::Parser parser1(ss1);
    BOOST_CHECK_EQUAL(JSONX::ValueType::JsonNull, parser1.checkValueType());
    std::shared_ptr<JSONX::IMPLEMENT::ValueNull> sp1(parser1.readValueNull());
    BOOST_CHECK(sp1 != nullptr && sp1->isNull());
    

    std::stringstream ss2(std::string("Null"));
    JSONX::IMPLEMENT::Parser parser2(ss2);
    BOOST_CHECK_EQUAL(JSONX::ValueType::JsonNull, parser2.checkValueType());
    std::shared_ptr<JSONX::IMPLEMENT::ValueNull> sp2(parser2.readValueNull());
    BOOST_CHECK(sp2 != nullptr && sp2->isNull());
}

BOOST_AUTO_TEST_CASE(CheckBoolean)
{
    std::stringstream ss1(std::string("true"));
    JSONX::IMPLEMENT::Parser parser1(ss1);
    BOOST_CHECK_EQUAL(JSONX::ValueType::JsonBoolean, parser1.checkValueType());
    std::shared_ptr<JSONX::IMPLEMENT::ValueBoolean> sp1(parser1.readValueBoolean());
    BOOST_CHECK(sp1 != nullptr && sp1->isBoolean() && sp1->get());

    std::istringstream ss2(std::string("false"));
    JSONX::IMPLEMENT::Parser parser2(ss2);
    BOOST_CHECK_EQUAL(JSONX::ValueType::JsonBoolean, parser2.checkValueType());
    std::shared_ptr<JSONX::IMPLEMENT::ValueBoolean> sp2(parser2.readValueBoolean());
    BOOST_CHECK(sp2 != nullptr && sp2->isBoolean() && !sp2->get());
}

BOOST_AUTO_TEST_CASE(CheckNumber)
{
    JSONX::IMPLEMENT::StringParser<char> parser("0");
    BOOST_CHECK_EQUAL(JSONX::ValueType::JsonNumber, parser.checkValueType());
    std::shared_ptr<JSONX::IMPLEMENT::ValueNumber> sp(parser.readValueNumber());
    BOOST_CHECK(sp != nullptr && sp->isNumber() && !sp->isDecimal() && !sp->isSigned());

    parser.reset("-100");
    BOOST_CHECK_EQUAL(JSONX::ValueType::JsonNumber, parser.checkValueType());
    sp.reset(parser.readValueNumber());
    BOOST_CHECK(sp != nullptr && sp->isNumber() && !sp->isDecimal() && sp->isSigned());

    parser.reset("-100.13892");
    BOOST_CHECK_EQUAL(JSONX::ValueType::JsonNumber, parser.checkValueType());
    sp.reset(parser.readValueNumber());
    BOOST_CHECK(sp != nullptr && sp->isNumber() && sp->isDecimal() && sp->isSigned());

    parser.reset("0.13892");
    BOOST_CHECK_EQUAL(JSONX::ValueType::JsonNumber, parser.checkValueType());
    sp.reset(parser.readValueNumber());
    BOOST_CHECK(sp != nullptr && sp->isNumber() && sp->isDecimal() && !sp->isSigned());
}

BOOST_AUTO_TEST_CASE(CheckString)
{
    JSONX::IMPLEMENT::StringParser<char> parser("\"\"");
    BOOST_CHECK_EQUAL(JSONX::ValueType::JsonString, parser.checkValueType());
    std::shared_ptr<JSONX::IMPLEMENT::ValueString> sp(parser.readValueString());
    BOOST_CHECK(sp != nullptr && sp->isString() && sp->getString() == std::string(""));

    parser.reset("\"Hello World!\"");
    BOOST_CHECK_EQUAL(JSONX::ValueType::JsonString, parser.checkValueType());
    sp.reset(parser.readValueString());
    BOOST_CHECK(sp != nullptr && sp->isString() && sp->getString() == std::string("Hello World!"));

    parser.reset("\"Hello\u0020\\\"World\\\"!\"");
    BOOST_CHECK_EQUAL(JSONX::ValueType::JsonString, parser.checkValueType());
    sp.reset(parser.readValueString());
    BOOST_CHECK(sp != nullptr && sp->isString() && sp->getString() == std::string("Hello \"World\"!"));
}

BOOST_AUTO_TEST_CASE(CheckObject)
{
    JSONX::IMPLEMENT::StringParser<char> parser("{}");
    BOOST_CHECK_EQUAL(JSONX::ValueType::JsonObject, parser.checkValueType());
    std::shared_ptr<JSONX::IMPLEMENT::ValueObject> sp(parser.readValueObject());
    BOOST_CHECK(sp != nullptr && sp->isObject() && sp->empty());

    parser.reset("{\"name\":\"John\",\"age\":20,\"employee\":true,\"score\":11.8912,\"data\":null}");
    BOOST_CHECK_EQUAL(JSONX::ValueType::JsonObject, parser.checkValueType());
    sp.reset(parser.readValueObject());
    BOOST_CHECK(sp != nullptr && sp->isObject() && !sp->empty() && sp->size() == 5);

    const JSONX::IMPLEMENT::ValueString* pName = dynamic_cast<const JSONX::IMPLEMENT::ValueString*>(sp->get("name").get());
    BOOST_CHECK_EQUAL(pName->getString(), "John");

    const JSONX::IMPLEMENT::ValueNumber* pAge = dynamic_cast<const JSONX::IMPLEMENT::ValueNumber*>(sp->get("AGE").get());
    BOOST_CHECK(pAge != nullptr && pAge->isNumber() && !pAge->isDecimal() && pAge->toUint32() == 20);

    const JSONX::IMPLEMENT::ValueBoolean* pEmployee = dynamic_cast<const JSONX::IMPLEMENT::ValueBoolean*>(sp->get("Employee").get());
    BOOST_CHECK(pEmployee != nullptr && pEmployee->isBoolean() && pEmployee->get());

    const JSONX::IMPLEMENT::ValueNumber* pScore = dynamic_cast<const JSONX::IMPLEMENT::ValueNumber*>(sp->get("Score").get());
    BOOST_CHECK(pScore != nullptr && pScore->isNumber() && pScore->isDecimal() && pScore->toUint32() == 11);

    const JSONX::IMPLEMENT::ValueNull* pData = dynamic_cast<const JSONX::IMPLEMENT::ValueNull*>(sp->get("data").get());
    BOOST_CHECK(pData != nullptr && pData->isNull());
}

BOOST_AUTO_TEST_SUITE_END()
