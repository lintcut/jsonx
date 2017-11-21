#include <Windows.h>
#include <jsonx/jsonx.hpp>

//#define BOOST_TEST_MODULE JSONX
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(JsonParserTest)

BOOST_AUTO_TEST_CASE(CheckNull)
{
    std::istringstream ss(std::string("null"));
    JSONX::IMPLEMENT::Parser parser(ss);
    BOOST_CHECK_EQUAL(JSONX::ValueType::JsonNull, parser.checkValueType());
#ifdef _DEBUG
    std::shared_ptr<JSONX::IMPLEMENT::ValueNull> sp(parser.readValueNull());
#else
    std::shared_ptr<JSONX::IMPLEMENT::ValueNull> sp(dynamic_cast<JSONX::IMPLEMENT::ValueNull*>(parser.readValue()));
#endif
    BOOST_CHECK(sp != nullptr && sp->isNull());

    ss.str("Null");
    parser.reset();
    BOOST_CHECK_EQUAL(JSONX::ValueType::JsonNull, parser.checkValueType());
#ifdef _DEBUG
    sp.reset(parser.readValueNull());
#else
    sp.reset(dynamic_cast<JSONX::IMPLEMENT::ValueNull*>(parser.readValue()));
#endif
    BOOST_CHECK(sp != nullptr && sp->isNull());
}

BOOST_AUTO_TEST_CASE(CheckBoolean)
{
    std::stringstream ss(std::string("true"));
    JSONX::IMPLEMENT::Parser parser(ss);
    BOOST_CHECK_EQUAL(JSONX::ValueType::JsonBoolean, parser.checkValueType());
#ifdef _DEBUG
    std::shared_ptr<JSONX::IMPLEMENT::ValueBoolean> sp(parser.readValueBoolean());
#else
    std::shared_ptr<JSONX::IMPLEMENT::ValueBoolean> sp(dynamic_cast<JSONX::IMPLEMENT::ValueBoolean*>(parser.readValue()));
#endif
    BOOST_CHECK(sp != nullptr && sp->isBoolean() && sp->get());

    ss.str("false");
    parser.reset();
    BOOST_CHECK_EQUAL(JSONX::ValueType::JsonBoolean, parser.checkValueType());
#ifdef _DEBUG
    sp.reset(parser.readValueBoolean());
#else
    sp.reset(dynamic_cast<JSONX::IMPLEMENT::ValueBoolean*>(parser.readValue()));
#endif
    BOOST_CHECK(sp != nullptr && sp->isBoolean() && !sp->get());
}

BOOST_AUTO_TEST_CASE(CheckNumber)
{
    JSONX::IMPLEMENT::StringParser<char> parser("0");
    BOOST_CHECK_EQUAL(JSONX::ValueType::JsonNumber, parser.checkValueType());
#ifdef _DEBUG
    std::shared_ptr<JSONX::IMPLEMENT::ValueNumber> sp(parser.readValueNumber());
#else
    std::shared_ptr<JSONX::IMPLEMENT::ValueNumber> sp(dynamic_cast<JSONX::IMPLEMENT::ValueNumber*>(parser.readValue()));
#endif
    BOOST_CHECK(sp != nullptr && sp->isNumber() && !sp->isDecimal() && !sp->isSigned());

    parser.reset("-100");
    BOOST_CHECK_EQUAL(JSONX::ValueType::JsonNumber, parser.checkValueType());
#ifdef _DEBUG
    sp.reset(parser.readValueNumber());
#else
    sp.reset(dynamic_cast<JSONX::IMPLEMENT::ValueNumber*>(parser.readValue()));
#endif
    BOOST_CHECK(sp != nullptr && sp->isNumber() && !sp->isDecimal() && sp->isSigned());

    parser.reset("-100.13892");
    BOOST_CHECK_EQUAL(JSONX::ValueType::JsonNumber, parser.checkValueType());
#ifdef _DEBUG
    sp.reset(parser.readValueNumber());
#else
    sp.reset(dynamic_cast<JSONX::IMPLEMENT::ValueNumber*>(parser.readValue()));
#endif
    BOOST_CHECK(sp != nullptr && sp->isNumber() && sp->isDecimal() && sp->isSigned());

    parser.reset("0.13892");
    BOOST_CHECK_EQUAL(JSONX::ValueType::JsonNumber, parser.checkValueType());
#ifdef _DEBUG
    sp.reset(parser.readValueNumber());
#else
    sp.reset(dynamic_cast<JSONX::IMPLEMENT::ValueNumber*>(parser.readValue()));
#endif
    BOOST_CHECK(sp != nullptr && sp->isNumber() && sp->isDecimal() && !sp->isSigned());
}

BOOST_AUTO_TEST_CASE(CheckString)
{
    JSONX::IMPLEMENT::StringParser<char> parser("\"\"");
    BOOST_CHECK_EQUAL(JSONX::ValueType::JsonString, parser.checkValueType());
#ifdef _DEBUG
    std::shared_ptr<JSONX::IMPLEMENT::ValueString> sp(parser.readValueString());
#else
    std::shared_ptr<JSONX::IMPLEMENT::ValueString> sp(dynamic_cast<JSONX::IMPLEMENT::ValueString*>(parser.readValue()));
#endif
    BOOST_CHECK(sp != nullptr && sp->isString() && sp->get() == std::string(""));

    parser.reset("\"Hello World!\"");
    BOOST_CHECK_EQUAL(JSONX::ValueType::JsonString, parser.checkValueType());
#ifdef _DEBUG
    sp.reset(parser.readValueString());
#else
    sp.reset(dynamic_cast<JSONX::IMPLEMENT::ValueString*>(parser.readValue()));
#endif
    BOOST_CHECK(sp != nullptr && sp->isString() && sp->get() == std::string("Hello World!"));

    parser.reset("\"Hello\u0020\\\"World\\\"!\"");
    BOOST_CHECK_EQUAL(JSONX::ValueType::JsonString, parser.checkValueType());
#ifdef _DEBUG
    sp.reset(parser.readValueString());
#else
    sp.reset(dynamic_cast<JSONX::IMPLEMENT::ValueString*>(parser.readValue()));
#endif
    BOOST_CHECK(sp != nullptr && sp->isString() && sp->get() == std::string("Hello \"World\"!"));
}

BOOST_AUTO_TEST_CASE(CheckObject)
{
    JSONX::IMPLEMENT::StringParser<char> parser("{}");
    BOOST_CHECK_EQUAL(JSONX::ValueType::JsonObject, parser.checkValueType());
#ifdef _DEBUG
    std::shared_ptr<JSONX::IMPLEMENT::ValueObject> sp(parser.readValueObject());
#else
    std::shared_ptr<JSONX::IMPLEMENT::ValueObject> sp(dynamic_cast<JSONX::IMPLEMENT::ValueObject*>(parser.readValue()));
#endif
    BOOST_CHECK(sp != nullptr && sp->isObject() && sp->empty());

    parser.reset("{\"name\":\"John\",\"age\":20,\"employee\":true,\"weight\":11.8912,\"data\":null,\"phone\":[\"+8613667423581\",\"650-863-0000\"],\"score\":{\"Math\":100,\"Language\":90,\"Art\":60}}");
    BOOST_CHECK_EQUAL(JSONX::ValueType::JsonObject, parser.checkValueType());
#ifdef _DEBUG
    sp.reset(parser.readValueObject());
#else
    sp.reset(dynamic_cast<JSONX::IMPLEMENT::ValueObject*>(parser.readValue()));
#endif
    BOOST_CHECK(sp != nullptr && sp->isObject() && !sp->empty() && sp->size() == 7);

    const JSONX::IMPLEMENT::ValueString* pName = dynamic_cast<const JSONX::IMPLEMENT::ValueString*>(sp->get("name").get());
    BOOST_CHECK_EQUAL(pName->get(), "John");

    const JSONX::IMPLEMENT::ValueNumber* pAge = dynamic_cast<const JSONX::IMPLEMENT::ValueNumber*>(sp->get("AGE").get());
    BOOST_CHECK(pAge != nullptr && pAge->isNumber() && !pAge->isDecimal() && pAge->toUint32() == 20);

    const JSONX::IMPLEMENT::ValueBoolean* pEmployee = dynamic_cast<const JSONX::IMPLEMENT::ValueBoolean*>(sp->get("Employee").get());
    BOOST_CHECK(pEmployee != nullptr && pEmployee->isBoolean() && pEmployee->get());

    const JSONX::IMPLEMENT::ValueNumber* pWeight = dynamic_cast<const JSONX::IMPLEMENT::ValueNumber*>(sp->get("Weight").get());
    BOOST_CHECK(pWeight != nullptr && pWeight->isNumber() && pWeight->isDecimal() && pWeight->toUint32() == 11);

    const JSONX::IMPLEMENT::ValueNull* pData = dynamic_cast<const JSONX::IMPLEMENT::ValueNull*>(sp->get("data").get());
    BOOST_CHECK(pData != nullptr && pData->isNull());

    const JSONX::IMPLEMENT::ValueArray* pPhone = dynamic_cast<const JSONX::IMPLEMENT::ValueArray*>(sp->get("phone").get());
    BOOST_CHECK(pPhone != nullptr && pPhone->isArray() && pPhone->size() == 2);

    const JSONX::IMPLEMENT::ValueObject* pScore = dynamic_cast<const JSONX::IMPLEMENT::ValueObject*>(sp->get("Score").get());
    BOOST_CHECK(pScore != nullptr && pScore->isObject() && pScore->size() == 3);
}


BOOST_AUTO_TEST_CASE(CheckArray)
{
    JSONX::IMPLEMENT::StringParser<char> parser("[]");
    BOOST_CHECK_EQUAL(JSONX::ValueType::JsonArray, parser.checkValueType());
#ifdef _DEBUG
    std::shared_ptr<JSONX::IMPLEMENT::ValueArray> sp(parser.readValueArray());
#else
    std::shared_ptr<JSONX::IMPLEMENT::ValueArray> sp(dynamic_cast<JSONX::IMPLEMENT::ValueArray*>(parser.readValue()));
#endif
    BOOST_CHECK(sp != nullptr && sp->isArray() && sp->empty());

    parser.reset("[\"John\",20,true,11.8912,null,[\"+8613667423581\",\"650-863-0000\"],{\"Math\":100,\"Language\":90,\"Art\":60}]");
    BOOST_CHECK_EQUAL(JSONX::ValueType::JsonArray, parser.checkValueType());
#ifdef _DEBUG
    sp.reset(parser.readValueArray());
#else
    sp.reset(dynamic_cast<JSONX::IMPLEMENT::ValueArray*>(parser.readValue()));
#endif
    BOOST_CHECK(sp != nullptr && sp->isArray() && !sp->empty() && sp->size() == 7);

    const JSONX::IMPLEMENT::ValueString* pName = dynamic_cast<const JSONX::IMPLEMENT::ValueString*>(sp->get(0).get());
    BOOST_CHECK_EQUAL(pName->get(), "John");

    const JSONX::IMPLEMENT::ValueNumber* pAge = dynamic_cast<const JSONX::IMPLEMENT::ValueNumber*>(sp->get(1).get());
    BOOST_CHECK(pAge != nullptr && pAge->isNumber() && !pAge->isDecimal() && pAge->toUint32() == 20);

    const JSONX::IMPLEMENT::ValueBoolean* pEmployee = dynamic_cast<const JSONX::IMPLEMENT::ValueBoolean*>(sp->get(2).get());
    BOOST_CHECK(pEmployee != nullptr && pEmployee->isBoolean() && pEmployee->get());

    const JSONX::IMPLEMENT::ValueNumber* pWeight = dynamic_cast<const JSONX::IMPLEMENT::ValueNumber*>(sp->get(3).get());
    BOOST_CHECK(pWeight != nullptr && pWeight->isNumber() && pWeight->isDecimal() && pWeight->toUint32() == 11);

    const JSONX::IMPLEMENT::ValueNull* pData = dynamic_cast<const JSONX::IMPLEMENT::ValueNull*>(sp->get(4).get());
    BOOST_CHECK(pData != nullptr && pData->isNull());

    const JSONX::IMPLEMENT::ValueArray* pPhone = dynamic_cast<const JSONX::IMPLEMENT::ValueArray*>(sp->get(5).get());
    BOOST_CHECK(pPhone != nullptr && pPhone->isArray() && pPhone->size() == 2);

    const JSONX::IMPLEMENT::ValueObject* pScore = dynamic_cast<const JSONX::IMPLEMENT::ValueObject*>(sp->get(6).get());
    BOOST_CHECK(pScore != nullptr && pScore->isObject() && pScore->size() == 3);
}

BOOST_AUTO_TEST_SUITE_END()
