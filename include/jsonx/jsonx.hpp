#pragma once
#ifndef JSONX_H
#define JSONX_H

#include <cstdint>
#include <string>
#include <memory>
#include <iostream>

namespace JSONX {

// class Forward
class Value;
class Null;
class Boolean;
class Number;
class String;
class Object;
class Array;

namespace Impl {
    class ValueImpl;
    class NullImpl;
    class BooleanImpl;
    class NumberImpl;
    class StringImpl;
    class ObjectImpl;
    class ArrayImpl;
}
    
template<typename CharType>
Value read(const std::basic_stream<CharType>& stm);

template<typename CharType>
Value read(const std::basic_string<CharType>& str);

class Value
{
public:
    Value();
    Value(Value&& rhs);
    virtual ~Value();

    // Copy is not allowed
    Value(const Value& rhs) = delete;
    Value& operator = (const Value& rhs) = delete;
    // Move is allowed
    Value& operator = (Value&& rhs)
    {
        if(this != &rhs)
        {
            valPtr = rhs.valPtr;
        }
        return *this;
    }

    inline bool empty() const { return (valPtr == nullptr); }
    inline bool isNull() const { return (!empty() && valPtr->isNull()); }
    inline bool isBoolean() const { return (!empty() && valPtr->isBoolean()); }
    inline bool isNumber() const { return (!empty() && valPtr->isNumber()); }
    inline bool isString() const { return (!empty() && valPtr->isString()); }
    inline bool isArray() const { return (!empty() && valPtr->isArray()); }
    inline bool isObject() const { return (!empty() && valPtr->isObject()); }

    // String

    
    virtual template<typename CharType> bool write(std::basic_stream<CharType>& stm) const;

    template<typename CharType>
    std::basic_string<CharType> serilize() const;

private:
    std::unique_ptr<Impl::ValueImpl> valPtr;
};

class Null : public Value
{
public:
    Null();
    virtual ~Null();
};

class Boolean : public Value
{
public:
    Boolean();
    virtual ~Boolean();
};

class Number : public Value
{
public:
    Number();
    virtual ~Number();
};

class String : public Value
{
public:
    String();
    virtual ~String();
};

class Array : public Value
{
public:
    Array();
    virtual ~Array();
};

class Object : public Value
{
public:
    Object();
    virtual ~Object();
};


namespace Impl {

class ValueImpl
{
public:
    ValueImpl();
    virtual ~ValueImpl();

    virtual bool isNull() const { return true; }
    virtual bool isBoolean() const { return false; }
    virtual bool isNumber() const { return false; }
    virtual bool isString() const { return false; }
    virtual bool isArray() const { return false; }
    virtual bool isObject() const { return false; }

    virtual
    template<typename CharType>
    void write(std::basic_stream<CharType>& stm) const;

    template<typename CharType>
    std::basic_string<CharType> serilize() const;
};

class NullImpl : public ValueImpl
{
public:
    NullImpl() {}
    virtual ~NullImpl() {}

    virtual
    template<typename CharType>
    void write(std::basic_stream<CharType>& stm) const
    {
        stm << CharType('N');
        stm << CharType('u');
        stm << CharType('l');
        stm << CharType('l');
    }
};

class BooleanImpl : public ValueImpl
{
public:
    BooleanImpl();
    virtual ~Boolean();

    virtual
    template<typename CharType>
    void write(std::basic_stream<CharType>& stm) const
    {
        if(val)
        {
            stm << CharType('t');
            stm << CharType('r');
            stm << CharType('u');
            stm << CharType('e');
        }
        else
        {
            stm << CharType('f');
            stm << CharType('a');
            stm << CharType('l');
            stm << CharType('s');
            stm << CharType('e');
        }
    }

private:
    bool val;
};

class NumberImpl : public ValueImpl
{
public:
    NumberImpl();
    virtual ~NumberImpl();

    inline bool isSigned() const { return signedNumber; }
    inline bool isFloat() const { return floatNumber; }

    inline int32_t toInt32() const { return isFloat() ? static_case<int32_t>(f) : l; }
    inline uint32_t toUint32() const { return isFloat() ? static_case<uint32_t>(f) : ul; }
    inline int64_t toInt64() const { return isFloat() ? static_case<int32_t>(f) : ll; }
    inline uint64_t toUint64() const { return isFloat() ? static_case<uint32_t>(f) : ull; }
    inline double toFloat() const { return isFloat() ? f : static_case<double>(ll*1.0); }
    
    inline operator int32_t() const { toInt32(); }
    inline operator uint32_t() const { toUint32(); }
    inline operator int64_t() const { toInt64(); }
    inline operator uint64_t() const { toUint64(); }
    inline operator double() const { toDouble(); }

private:
    union {
        uint32_t ul;
        int32_t l;
        uint64_t ull;
        int64_t ll;
        double f;
    };
    bool signedNumber;
    bool floatNumber;
};

class StringImpl : public ValueImpl
{
public:
    StringImpl();
    virtual ~StringImpl();
};

class ArrayImpl : public ValueImpl
{
public:
    ArrayImpl();
    virtual ~ArrayImpl();
};

class ObjectImpl : public ValueImpl
{
public:
    ObjectImpl();
    virtual ~ObjectImpl();
};

}   // namespace Impl




}   // namespace JSONX


#endif
