# JSONX #

A simple, very easy to use C++ class, which allow you to parse json string or file and manipulate json values.

## Table of Content

- [1. Overview](#1-overview)
- [2. Include](#2-include)
- [3. Namespace](#3-namespace)
- [4. Classes](#4-classes)
    - [4.1 class **Value**](#41-class-value)
        - [4.1.1 Constructor and Deconstructor](#411-constructor-and-deconstructor)
        - [4.1.2 Parser](#412-parser)
        - [4.1.3 Type Check](#413-type-check)
        - [4.1.4 Get Value](#414-get-value)
        - [4.1.5 Set Value](#415-set-value)
        - [4.1.6 Misc](#416-misc)
    - [4.2 class **SerializeConfig**](#42-class-serializeconfig)
- [5. Examples](#5-examples)
    - [5.1 Parsing](#51-parsing)
    - [5.2 Serialization](#52-serialization)
        - [5.2.1 Simple](#521-simple)
        - [5.2.2 Well-formatted](#522-well-formatted)
    - [5.3 Create](#53-create)

## 1. Overview

## 2. Include

All JSONX implementation is in a header file - `<jsonx/jsonx.hpp>`. To use JSONX, this header file must be included first.

## 3. Namespace

All the implementation is under namespace `JSONX`.

```cpp
using namespace JSONX;
```

## 4. Classes

### 4.1 class **Value**

#### 4.1.1 Constructor and Deconstructor

`class Value` provides several useful constructors to allow user create different JSON value easily.

```cpp
Value::Value();
// Copy construct
Value::Value(const Value& rhs);
// JsonBoolean Value
explicit Value::Value(bool v);
// JsonNumber Value
explicit Value::Value(int32_t v);
explicit Value::Value(int64_t v);
explicit Value::Value(uint32_t v);
explicit Value::Value(uint64_t v);
explicit Value::Value(float_t v);
explicit Value::Value(double_t v);
// JsonString Value
explicit Value::Value(const std::string& v, bool escaped=false);
explicit Value::Value(const std::wstring& v, bool escaped=false);
// Construct from pointer
explicit Value::Value(std::shared_ptr<IMPLEMENT::ValueBase> p);
// Construct from type
explicit Value::Value(ValueType vt);
// Deconstructor
Value::~Value();
// Operator =
Value& Value::operator = (const Value& rhs);
```

#### 4.1.2 Parser

`class Value` provides 3 **`static`** functions to parse JSON string/file. If the call succeeds, a valid `Value` object is returned. Otherwise, it returns an invalid `Value` object.

```cpp
static Value Value::parse(const std::string& s);
static Value Value::parse(const std::wstring& s);
static Value Value::parseFile(const std::string& path);
```

#### 4.1.3 Type Check

Following functions check `Value` object's type.

```cpp
bool Value::isNull() const;
bool Value::isBoolean() const;
bool Value::isNumber() const;
bool Value::isString() const;
bool Value::isObject() const;
bool Value::isArray() const;
bool Value::isSignedNumber() const;
bool Value::isIntegerNumber() const;
bool Value::isDecimalNumber() const;
```

#### 4.1.4 Get Value

Following functions get data value of `Value` object.

```cpp
// Get JsonBoolean value
bool Value::getBoolean() const;
// Get JsonNumber value
int32_t Value::getInt32() const;
int64_t Value::getInt64() const;
uint32_t Value::getUint32() const;
uint64_t Value::getUint64() const;
double_t Value::getDecimal() const;
// Get JsonString value
const std::string& Value::getString() const;
const std::string& Value::getWstring() const;
// Get JsonObject value
Value Value::operator [](const std::string& key);
const Value Value::operator [](const std::string& key) const;
// Get JsonArray value
Value Value::operator [](size_t id);
const Value Value::operator [](size_t id) const;
```

#### 4.1.5 Set Value

Following functions set data value of `Value` object.

```cpp
// Set JsonBoolean value
void Value::set(bool v);
// Set JsonNumber value
void Value::set(int32_t v);
void Value::set(int64_t v);
void Value::set(uint32_t v);
void Value::set(uint64_t v);
void Value::set(float_t v);
void Value::set(double_t v);
// Set JsonString value
void Value::set(const std::string& v);
void Value::set(const std::wstring& v);
// Add/Set JsonObject sub-item
Value Value::set(const std::string& key, Value& v);
Value Value::set(const std::string& key, bool v);
Value Value::set(const std::string& key, int32_t v);
Value Value::set(const std::string& key, int64_t v);
Value Value::set(const std::string& key, uint32_t v);
Value Value::set(const std::string& key, uint64_t v);
Value Value::set(const std::string& key, float_t v);
Value Value::set(const std::string& key, double_t v);
Value Value::set(const std::string& key, const std::string& v);
Value Value::set(const std::string& key, const std::wstring& v);
// Add JsonArray sub-item
Value Value::push_back(Value& v);
Value Value::push_back(bool v);
Value Value::push_back(int32_t v);
Value Value::push_back(int64_t v);
Value Value::push_back(uint32_t v);
Value Value::push_back(uint64_t v);
Value Value::push_back(float_t v);
Value Value::push_back(double_t v);
Value Value::push_back(const std::string& v);
Value Value::push_back(const std::wstring& v);
```

#### 4.1.6 Misc

```cpp
// Check if this is a valid Value object
bool Value::valid() const;
// Get value size
bool Value::size() const;
// Serialize Value object
std::string Value::serialize(SerializeConfig* config = nullptr) const;
```

### 4.2 class **SerializeConfig**

This class define a serialization config object which is used by `Value::serialize()` function.

```cpp
// Default config object:
//   -> No format (compact string)
//   -> Line ending is LF ('\n')
SerializeConfig::SerializeConfig();
// Customized config object:
//   -> formatted: true or false
//   -> eof: LF or CRLF
SerializeConfig::SerializeConfig(bool formatted, const char* eol="\n");
```

## 5. Examples

### 5.1 Parsing

```cpp
#include <jsonx/jsonx.hpp>
using namespace JSONX;

// Parse a file
void parseExample1()
{
    const Value& root = Value::parseFile("C:\\test\\sample.json");
}
// Parse a string
void parseExample2()
{
    const Value& root = Value::parse("{\"name\":\"John Tyler\",\"age\":28}");
}
// Parse a wstring
void parseExample3()
{
    const Value& root = Value::parse(L"{\"name\":\"John Tyler\",\"age\":28}");
}
```

### 5.2 Serialization

### 5.2.1 Simple

```cpp
#include <jsonx/jsonx.hpp>
#include <iostream>
using namespace JSONX;

void serializeExample2()
{
    const Value& root = Value::parse("{\"name\":\"John Tyler\",\"age\":28,\"scores\":[99,86,100,78]}");
    const std::string& s = root.serialize();
    std::cout << s << sytd::endl;
}
```

**Output JSON string:**

```json
{"name":"John Tyler","age":28,"scores":[99,86,100,78]}
```

### 5.2.2 Well-formatted

```cpp
#include <jsonx/jsonx.hpp>
#include <iostream>
using namespace JSONX;

void serializeExample2()
{
    const Value& root = Value::parse("{\"name\":\"John Tyler\",\"age\":28,  \"scores\":[99,86,100,78]}");
    SerializeConfig config(true, "\n");
    const std::string& s = root.serialize(&config);
    std::cout << s << sytd::endl;
}
```

**Output JSON string:**

```json
{
    "name":"John Tyler",
    "age":28,
    "scores": [
        99,
        86,
        100,
        78
    ]
}
```


### 5.3 Create

For example, try to create following JSON object

```json
{
    "name":"John Tyler",
    "age":28,
    "scores": [
        99,
        86,
        100,
        78
    ]
}
```
 **Code:**
 
```cpp
#include <jsonx/jsonx.hpp>
using namespace JSONX;

void createExample()
{
    Value root(JsonObject);
    root.set("name", "John Tyler");
    root.set("age", 28);
    Value scores = root.set("scores", Value(JsonArray));
    scores.push_back(99);
    scores.push_back(86);
    scores.push_back(100);
    scores.push_back(78);
}
```
