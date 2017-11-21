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

static const char* json1 = "{ \
    \"user\": { \
        \"id\": 18921,  \
        \"premier\": true,  \
        \"name\": \"John Tyler\",  \
        \"email\": \"john.tyler@gmail.com\",  \
        \"company\": \"\",  \
        \"country\": {  \
            \"name\": \"United States\",  \
            \"code\": \"US\"  \
        }, \
        \"phone\": {  \
            \"home\": \"650-756-8210\",  \
            \"mobile\": \"408-312-9527\"  \
        } \
    }, \
    \"ticket\": { \
        \"token\": \"6282AF8D6282AF8D6282AF8D6282AF8D6282AF8D6282AF8D6282AF8D6282AF8D\",  \
        \"expireTime\": 1892183648,  \
        \"issuer\": \"Xiang Ye\",  \
        \"issuedTime\": 1892183110,  \
        \"rights\": [  \
            \"query\",  \
            \"read\",  \
            \"write\"  \
        ] \
    }, \
    \"repositories\": [ \
        { \
            \"id\": 0,  \
            \"name\": \"Default\",  \
            \"provider\": \"Default\",  \
            \"url\": \"https:\\/\\/drive.default.com\\/user\\/query?id=1723382\",  \
            \"permission\": \"rwx\",  \
        }, \
        { \
            \"id\": 1,  \
            \"name\": \"Private Storage 1\",  \
            \"provider\": \"Google Drive\",  \
            \"url\": \"https:\\/\\/drive.google.com\\/user\\/query?id=1723382\",  \
            \"permission\": \"rw\",  \
        }, \
        { \
            \"id\": 2,  \
            \"name\": \"Dropbox Storage 1\",  \
            \"provider\": \"Dropbox\",  \
            \"url\": \"https:\\/\\/drive.dropbox.com\\/user\\/query?id=1723382\",  \
            \"permission\": \"r\",  \
        } \
    ] \
}";

static void checkJson1(const JSONX::Value& val)
{
    BOOST_CHECK(val.isObject());

    const JSONX::Value& valUser = val["user"];
    BOOST_CHECK(valUser.isObject());
    const JSONX::Value& valUserId = valUser["id"];
    BOOST_CHECK(valUserId.isNumber() && valUserId.isIntegerNumber());
    BOOST_CHECK_EQUAL(valUserId.getInt32(), 18921);
    const JSONX::Value& valUserPremier = valUser["premier"];
    BOOST_CHECK(valUserPremier.isBoolean() && valUserPremier.getBoolean());
    const JSONX::Value& valUserName = valUser["name"];
    BOOST_CHECK(valUserName.isString());
    BOOST_CHECK_EQUAL(valUserName.getString(), "John Tyler");
    const JSONX::Value& valUserEmail = valUser["email"];
    BOOST_CHECK(valUserEmail.isString());
    BOOST_CHECK_EQUAL(valUserEmail.getString(), "john.tyler@gmail.com");
    const JSONX::Value& valUserCompany = valUser["company"];
    BOOST_CHECK(valUserCompany.isString());
    BOOST_CHECK_EQUAL(valUserCompany.getString(), "");
    const JSONX::Value& valUserCountry = valUser["country"];
    BOOST_CHECK(valUserCountry.isObject());
    const JSONX::Value& valUserCountryName = valUserCountry["name"];
    BOOST_CHECK(valUserCountryName.isString());
    BOOST_CHECK_EQUAL(valUserCountryName.getString(), "United States");
    const JSONX::Value& valUserCountryCode = valUserCountry["code"];
    BOOST_CHECK(valUserCountryCode.isString());
    BOOST_CHECK_EQUAL(valUserCountryCode.getString(), "US");
    const JSONX::Value& valUserPhone = valUser["phone"];
    BOOST_CHECK(valUserPhone.isObject());
    const JSONX::Value& valUserPhoneHome = valUserPhone["home"];
    BOOST_CHECK(valUserPhoneHome.isString());
    BOOST_CHECK_EQUAL(valUserPhoneHome.getString(), "650-756-8210");
    const JSONX::Value& valUserPhoneMobile = valUserPhone["mobile"];
    BOOST_CHECK(valUserPhoneMobile.isString());
    BOOST_CHECK_EQUAL(valUserPhoneMobile.getString(), "408-312-9527");

    const JSONX::Value& valTicket = val["ticket"];
    BOOST_CHECK(valTicket.isObject());
    const JSONX::Value& valTicketToken = valTicket["token"];
    BOOST_CHECK(valTicketToken.isString());
    BOOST_CHECK_EQUAL(valTicketToken.getString(), "6282AF8D6282AF8D6282AF8D6282AF8D6282AF8D6282AF8D6282AF8D6282AF8D");
    const JSONX::Value& valTicketExpireTime = valTicket["expireTime"];
    BOOST_CHECK(valTicketExpireTime.isNumber());
    BOOST_CHECK_EQUAL(valTicketExpireTime.getInt64(), 1892183648);
    const JSONX::Value& valTicketIssuer = valTicket["issuer"];
    BOOST_CHECK(valTicketIssuer.isString());
    BOOST_CHECK_EQUAL(valTicketIssuer.getString(), "Xiang Ye");
    const JSONX::Value& valTicketIssuedTime = valTicket["issuedTime"];
    BOOST_CHECK(valTicketIssuedTime.isNumber());
    BOOST_CHECK_EQUAL(valTicketIssuedTime.getInt64(), 1892183110);
    const JSONX::Value& valTicketRights = valTicket["rights"];
    BOOST_CHECK(valTicketRights.isArray());
    BOOST_CHECK_EQUAL(valTicketRights.size(), 3);
    const JSONX::Value& valTicketRights0 = valTicketRights[0];
    BOOST_CHECK(valTicketRights0.isString());
    BOOST_CHECK_EQUAL(valTicketRights0.getString(), "query");
    const JSONX::Value& valTicketRights1 = valTicketRights[1];
    BOOST_CHECK(valTicketRights1.isString());
    BOOST_CHECK_EQUAL(valTicketRights1.getString(), "read");
    const JSONX::Value& valTicketRights2 = valTicketRights[2];
    BOOST_CHECK(valTicketRights2.isString());
    BOOST_CHECK_EQUAL(valTicketRights2.getString(), "write");

    const JSONX::Value& valRepos = val["repositories"];
    BOOST_CHECK(valRepos.isArray());
    BOOST_CHECK_EQUAL(valRepos.size(), 3);
    const JSONX::Value& valRepo0 = valRepos[0];
    BOOST_CHECK(valRepo0.isObject());
    BOOST_CHECK_EQUAL(valRepo0.size(), 5);
    const JSONX::Value& valRepo0Id = valRepo0["id"];
    BOOST_CHECK(valRepo0Id.isNumber());
    BOOST_CHECK_EQUAL(valRepo0Id.getInt64(), 0);
    const JSONX::Value& valRepo0Name = valRepo0["name"];
    BOOST_CHECK(valRepo0Name.isString());
    BOOST_CHECK_EQUAL(valRepo0Name.getString(), "Default");
    const JSONX::Value& valRepo0Provider = valRepo0["provider"];
    BOOST_CHECK(valRepo0Provider.isString());
    BOOST_CHECK_EQUAL(valRepo0Provider.getString(), "Default");
    const JSONX::Value& valRepo0Url = valRepo0["url"];
    BOOST_CHECK(valRepo0Url.isString());
    BOOST_CHECK_EQUAL(valRepo0Url.getString(), "https://drive.default.com/user/query?id=1723382");
    const JSONX::Value& valRepo0Permission = valRepo0["permission"];
    BOOST_CHECK(valRepo0Permission.isString());
    BOOST_CHECK_EQUAL(valRepo0Permission.getString(), "rwx");

    const JSONX::Value& valRepo1 = valRepos[1];
    BOOST_CHECK(valRepo1.isObject());
    BOOST_CHECK_EQUAL(valRepo1.size(), 5);
    const JSONX::Value& valRepo1Id = valRepo1["id"];
    BOOST_CHECK(valRepo1Id.isNumber());
    BOOST_CHECK_EQUAL(valRepo1Id.getInt64(), 1);
    const JSONX::Value& valRepo1Name = valRepo1["name"];
    BOOST_CHECK(valRepo1Name.isString());
    BOOST_CHECK_EQUAL(valRepo1Name.getString(), "Private Storage 1");
    const JSONX::Value& valRepo1Provider = valRepo1["provider"];
    BOOST_CHECK(valRepo1Provider.isString());
    BOOST_CHECK_EQUAL(valRepo1Provider.getString(), "Google Drive");
    const JSONX::Value& valRepo1Url = valRepo1["url"];
    BOOST_CHECK(valRepo1Url.isString());
    BOOST_CHECK_EQUAL(valRepo1Url.getString(), "https://drive.google.com/user/query?id=1723382");
    const JSONX::Value& valRepo1Permission = valRepo1["permission"];
    BOOST_CHECK(valRepo1Permission.isString());
    BOOST_CHECK_EQUAL(valRepo1Permission.getString(), "rw");

    const JSONX::Value& valRepo2 = valRepos[2];
    BOOST_CHECK(valRepo2.isObject());
    BOOST_CHECK_EQUAL(valRepo2.size(), 5);
    const JSONX::Value& valRepo2Id = valRepo2["id"];
    BOOST_CHECK(valRepo2Id.isNumber());
    BOOST_CHECK_EQUAL(valRepo2Id.getInt64(), 2);
    const JSONX::Value& valRepo2Name = valRepo2["name"];
    BOOST_CHECK(valRepo2Name.isString());
    BOOST_CHECK_EQUAL(valRepo2Name.getString(), "Dropbox Storage 1");
    const JSONX::Value& valRepo2Provider = valRepo2["provider"];
    BOOST_CHECK(valRepo2Provider.isString());
    BOOST_CHECK_EQUAL(valRepo2Provider.getString(), "Dropbox");
    const JSONX::Value& valRepo2Url = valRepo2["url"];
    BOOST_CHECK(valRepo2Url.isString());
    BOOST_CHECK_EQUAL(valRepo2Url.getString(), "https://drive.dropbox.com/user/query?id=1723382");
    const JSONX::Value& valRepo2Permission = valRepo2["permission"];
    BOOST_CHECK(valRepo2Permission.isString());
    BOOST_CHECK_EQUAL(valRepo2Permission.getString(), "r");
}

BOOST_AUTO_TEST_CASE(CheckValueParserA)
{
    const JSONX::Value& val = JSONX::Value::parse(json1);
    checkJson1(val);
}

BOOST_AUTO_TEST_CASE(CheckValueParserW)
{
    const std::string sJson1(json1);
    const std::wstring wsJson1(sJson1.begin(), sJson1.end());
    const JSONX::Value& val = JSONX::Value::parse(wsJson1);
    checkJson1(val);
}

BOOST_AUTO_TEST_CASE(CheckValueParserCompactedFile)
{
    const JSONX::Value& val = JSONX::Value::parse(json1);
    const std::string& s = val.serialize();
    BOOST_CHECK(!s.empty());

    std::ofstream ofs;
    ofs.open(L"test.json", std::ofstream::out | std::ofstream::trunc);
    BOOST_CHECK(ofs.is_open());
    ofs << s;
    ofs.close();

    const JSONX::Value& val2 = JSONX::Value::parseFile(L"test.json");
    checkJson1(val2);
#ifndef _DEBUG
    DeleteFileW(L"test.json");
#endif
}

BOOST_AUTO_TEST_CASE(CheckValueParserFormattedFile)
{
    const JSONX::Value& val = JSONX::Value::parse(json1);
    JSONX::SerializeConfig sc(true);
    const std::string& s = val.serialize(&sc);
    BOOST_CHECK(!s.empty());

    std::ofstream ofs;
    ofs.open(L"test-formatted.json", std::ofstream::out | std::ofstream::trunc);
    BOOST_CHECK(ofs.is_open());
    ofs << s;
    ofs.close();

    const JSONX::Value& val2 = JSONX::Value::parseFile(L"test-formatted.json");
    checkJson1(val2);

#ifndef _DEBUG
    DeleteFileW(L"test-formatted.json");
#endif
}

BOOST_AUTO_TEST_SUITE_END()
