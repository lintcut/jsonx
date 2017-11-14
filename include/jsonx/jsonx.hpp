#pragma once
#ifndef JSONX_H
#define JSONX_H

#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include <iostream>
#include <locale>
#include <codecvt>
#include <cstdlib>

#ifndef NOTHING
#define NOTHING
#endif

//
//  References:
//      RFC 7159 [The JavaScript Object Notation (JSON) Data Interchange Format]:  https://tools.ietf.org/html/rfc7159
//

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
}
    
class Value
{
public:
    typedef enum ValueType {
        JsonNull = 0,
        JsonBoolean,
        JsonNumber,
        JsonString,
        JsonArray,
        JsonObject
    } ValueType;

    Value()
    {
    }

    Value(const Value& rhs)
        : valImpl(rhs.valImpl)
    {
    }

    Value(Value&& rhs)
        : valImpl(std::move(rhs.valImpl))
    {
    }

    explicit Value(ValueType t)
        : valImpl(std::shared_ptr<Impl::ValueImpl>(
            JsonNull == t ? dynamic_cast<Impl::ValueImpl*>(new Impl::NullImpl())
            : (JsonBoolean == t ? dynamic_cast<Impl::ValueImpl*>(new Impl::BooleanImpl(false))
            : (JsonNumber == t ? dynamic_cast<Impl::ValueImpl*>(new Impl::NumberImpl(0ULL))
            : (JsonArray == t ? dynamic_cast<Impl::ValueImpl*>(new Impl::ArrayImpl())
            : (JsonObject == t ? dynamic_cast<Impl::ValueImpl*>(new Impl::ObjectImpl(true)) : nullptr))))
            ))
    {
    }

    explicit Value(bool v)
        : valImpl(std::shared_ptr<Impl::ValueImpl>(new Impl::BooleanImpl(v)))
    {
    }

    explicit Value(int32_t v)
        : valImpl(std::shared_ptr<Impl::ValueImpl>(new Impl::NumberImpl(v)))
    {
    }

    explicit Value(int64_t v)
        : valImpl(std::shared_ptr<Impl::ValueImpl>(new Impl::NumberImpl(v)))
    {
    }

    explicit Value(uint32_t v)
        : valImpl(std::shared_ptr<Impl::ValueImpl>(new Impl::NumberImpl(v)))
    {
    }

    explicit Value(uint64_t v)
        : valImpl(std::shared_ptr<Impl::ValueImpl>(new Impl::NumberImpl(v)))
    {
    }

    explicit Value(float v)
        : valImpl(std::shared_ptr<Impl::ValueImpl>(new Impl::NumberImpl(v)))
    {
    }

    explicit Value(double v)
        : valImpl(std::shared_ptr<Impl::ValueImpl>(new Impl::NumberImpl(v)))
    {
    }

    explicit Value(const std::string& v, bool escaped = false)
        : valImpl(std::shared_ptr<Impl::ValueImpl>(new Impl::StringImpl(v, escaped)))
    {
    }

    explicit Value(const char* v, bool escaped = false)
        : valImpl(std::shared_ptr<Impl::ValueImpl>(new Impl::StringImpl(std::string(v), escaped)))
    {
    }

    explicit Value(const std::wstring& v, bool escaped = false)
        : valImpl(std::shared_ptr<Impl::ValueImpl>(new Impl::StringImpl(v, escaped)))
    {
    }

    explicit Value(const wchar_t* v, bool escaped = false)
        : valImpl(std::shared_ptr<Impl::ValueImpl>(new Impl::StringImpl(std::wstring(v), escaped)))
    {
    }

    virtual ~Value()
    {
    }

    // Copy
    Value& operator = (const Value& rhs)
    {
        if(this != &rhs)
        {
            valImpl = rhs.valImpl;
        }
        return *this;
    }
    // Move
    Value& operator = (Value&& rhs)
    {
        if(this != &rhs)
        {
            valImpl = std::move(rhs.valImpl);
        }
        return *this;
    }

    inline bool empty() const { return (valImpl == nullptr); }
    inline bool isNull() const { return (!empty() && valImpl->isNull()); }
    inline bool isBoolean() const { return (!empty() && valImpl->isBoolean()); }
    inline bool isNumber() const { return (!empty() && valImpl->isNumber()); }
    inline bool isString() const { return (!empty() && valImpl->isString()); }
    inline bool isArray() const { return (!empty() && valImpl->isArray()); }
    inline bool isObject() const { return (!empty() && valImpl->isObject()); }

    inline Null asNull() { return Null(*this); }
    inline const Null& asNull() const { return Null(*this); }
    inline Boolean asBoolean() { return Boolean(*this); }
    inline const Boolean& asBoolean() const { return Boolean(*this); }
    inline Number asNumber() { return Number(*this); }
    inline const Number& asNumber() const { return Number(*this); }
    inline String asString() { return String(*this); }
    inline const String& asString() const { return String(*this); }
    inline Array asArray() { return Array(*this); }
    inline const Array& asArray() const { return Array(*this); }
    inline Object asObject() { return Object(*this); }
    inline const Object& asObject() const { return Object(*this); }

    bool write(std::ostream& stm) const
    {
        if(empty())
            return false;
        return valImpl->write(stm);
    }

    bool read(std::istream& stm)
    {
        valImpl = Impl::ValueImpl::read(stm);
        return (valImpl != nullptr);
    }

protected:
    explicit Value(std::shared_ptr<Impl::ValueImpl> sp)
        : valImpl(sp)
    {
    }
    std::shared_ptr<Impl::ValueImpl> getImpl() { return valImpl; }
    std::shared_ptr<Impl::ValueImpl> getImpl() const { return valImpl; }

private:
    std::shared_ptr<Impl::ValueImpl> valImpl;
    friend class Array;
    friend class Object;
};

class Null : public Value
{
public:
    virtual ~Null() {}

protected:
    Null() : Value(JsonNull)
    {
    }

    Null(const Value& val)
        : Value(val.isNull() ? val : Value(JsonNull))
    {
    }

private:
    friend class Value;
};

class Boolean : public Value
{
public:
    virtual ~Boolean() {}

    operator bool() const {return dynamic_cast<Impl::BooleanImpl*>(getImpl().get())->get();}
    void operator = (bool v) {dynamic_cast<Impl::BooleanImpl*>(getImpl().get())->set(v);}

protected:
    Boolean() : Value(JsonBoolean)
    {
    }
    Boolean(const Value& val)
        : Value(val.isBoolean() ? val : Value(JsonBoolean))
    {
    }

private:
    friend class Value;
};

class Number : public Value
{
public:
    virtual ~Number() {}

    inline bool isSigned() const { return getRef().isSigned(); }
    inline bool isFloat() const { return getRef().isFloat(); }

    inline int32_t toInt32() const { return getRef().toInt32(); }
    inline uint32_t toUint32() const { return getRef().toUint32(); }
    inline int64_t toInt64() const { return getRef().toInt64(); }
    inline uint64_t toUint64() const { return getRef().toUint64(); }
    inline double toDecimal() const { return getRef().toDecimal(); }

    inline operator int32_t() const { toInt32(); }
    inline operator uint32_t() const { toUint32(); }
    inline operator int64_t() const { toInt64(); }
    inline operator uint64_t() const { toUint64(); }
    inline operator float() const { static_cast<float>(toDecimal()); }
    inline operator double() const { toDecimal(); }

protected:
    Number() : Value(JsonNumber)
    {
    }
    Number(const Value& val)
        : Value(val.isNumber() ? val : Value(JsonNumber))
    {
    }

    Impl::NumberImpl& getRef()
    {
        return *dynamic_cast<Impl::NumberImpl*>(getImpl().get());
    }

    const Impl::NumberImpl& getRef() const
    {
        return *dynamic_cast<Impl::NumberImpl*>(getImpl().get());
    }

private:
    friend class Value;
};

class String : public Value
{
public:
    virtual ~String() {}

    operator const char* () const { dynamic_cast<Impl::StringImpl*>(getImpl().get())->get().c_str(); }
    operator const std::string& () const { dynamic_cast<Impl::StringImpl*>(getImpl().get())->get(); }
    void operator = (const std::string& s) { dynamic_cast<Impl::StringImpl*>(getImpl().get())->set(s, false); }
    void operator = (const std::wstring& s) { dynamic_cast<Impl::StringImpl*>(getImpl().get())->set(s, false); }

protected:
    String() : Value(JsonString)
    {
    }
    String(const Value& val)
        : Value(val.isString() ? val : Value(JsonString))
    {
    }

private:
    friend class Value;
};

class Array : public Value
{
public:
    virtual ~Array() {}

    void push_back(const Value& v)
    {
        dynamic_cast<Impl::ArrayImpl*>(getImpl().get())->push_back(v.getImpl());
    }

    Value operator [] (size_t index)
    {
        return empty() ? Value() : Value(getRef()[index]);
    }

    const Value& operator [] (size_t index) const
    {
        return empty() ? Value() : Value(getRef()[index]);
    }

protected:
    Array() : Value(JsonArray)
    {
    }
    Array(const Value& val)
        : Value(val.isArray() ? val : Value(JsonArray))
    {
    }

    Impl::ArrayImpl& getRef()
    {
        return *dynamic_cast<Impl::ArrayImpl*>(getImpl().get());
    }

    const Impl::ArrayImpl& getRef() const
    {
        return *dynamic_cast<const Impl::ArrayImpl*>(getImpl().get());
    }

private:
    friend class Value;
};

class Object : public Value
{
public:
    virtual ~Object() {}

    void set(const std::string& key, const Value& v)
    {
        dynamic_cast<Impl::ObjectImpl*>(getImpl().get())->set(key, v.getImpl());
    }

    Value operator [] (const std::string& key)
    {
        return empty() ? Value() : Value(getRef()[key]);
    }

    const Value& operator [] (const std::string& key) const
    {
        return empty() ? Value() : Value(getRef()[key]);
    }

protected:
    Object() : Value(JsonObject) {}
    Object(const Value& val)
        : Value(val.isObject() ? val : Value(JsonObject))
    {
    }

    Impl::ObjectImpl& getRef()
    {
        return *dynamic_cast<Impl::ObjectImpl*>(getImpl().get());
    }

    const Impl::ObjectImpl& getRef() const
    {
        return *dynamic_cast<const Impl::ObjectImpl*>(getImpl().get());
    }

private:
    friend class Value;
};


namespace Impl {

class NullImpl;
class BooleanImpl;
class NumberImpl;
class StringImpl;
class ArrayImpl;
class ObjectImpl;

namespace Utils {

    std::string toUtf8(const std::wstring& s)
    {
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> convert;
        return convert.to_bytes(s);    
    }

    std::wstring toUtf16(const std::string& s)
    {
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> convert;
        return convert.from_bytes(s); 
    }

    // JSON escape/unescape rules:
    //  https://tools.ietf.org/html/rfc7159#page-8
    std::string escape(const std::string& s)
    {
        std::string s2;
        const char* pos = s.c_str();
        while(*pos)
        {
            switch(*pos)
            {
            case '\\':
                s2.append("\\\\");
                break;
            case '/':
            s2.append("\\/");
                break;
            case '\"':
                s2.append("\\\"");
                break;
            case '\b':
                s2.append("\\b");
                break;
            case '\f':
                s2.append("\\f");
                break;
            case '\t':
                s2.append("\\t");
                break;
            case '\r':
                s2.append("\\r");
                break;
            case '\n':
                s2.append("\\n");
                break;
            default:
                s2.append(pos, 1);
                break;
            }
        }
        return std::move(s2);
    }

    std::string unescape(const std::string& s)
    {
        std::string s2;
        const char* pos = s.c_str();
        while(*pos)
        {
            if(*pos == '\\')
            {
                // escaped character
                ++pos;
                if(*pos == '\\')
                {
                    s2.append("\\");
                    ++pos;
                }
                else if (*pos == '/')
                {
                    s2.append("/");
                    ++pos;
                }
                else if (*pos == '\"')
                {
                    s2.append("\"");
                    ++pos;
                }
                else if (*pos == 'b')
                {
                    s2.append("\b");
                    ++pos;
                }
                else if (*pos == 'f')
                {
                    s2.append("\f");
                    ++pos;
                }
                else if (*pos == 't')
                {
                    s2.append("\t");
                    ++pos;
                }
                else if (*pos == 'r')
                {
                    s2.append("\r");
                    ++pos;
                }
                else if (*pos == 'n')
                {
                    s2.append("\n");
                    ++pos;
                }
                else if (*pos == 'u')
                {
                    ++pos;
                    // Next four digits must be hex
                    if(isxdigit(pos[0]) && isxdigit(pos[1]) && isxdigit(pos[2]) && isxdigit(pos[3]))
                    {
                        std::string sHex(pos, 4);
                        const wchar_t wc = (wchar_t)std::strtoul(sHex.c_str(), nullptr, 16);
                        s2.append(toUtf8(std::wstring(&wc, 1)));
                        pos += 4;
                    }
                    else
                    {
                        // Error happened
                        break;
                    }
                }
                else
                {
                    s2.append(pos, 1);
                    ++pos;
                }
            }
            else
            {
                s2.append(pos, 1);
                ++pos;
            }
        }
        return std::move(s2);
    }
}

class ValueImpl
{
public:
    virtual ~ValueImpl() {}

    virtual bool isNull() const { return true; }
    virtual bool isBoolean() const { return false; }
    virtual bool isNumber() const { return false; }
    virtual bool isString() const { return false; }
    virtual bool isArray() const { return false; }
    virtual bool isObject() const { return false; }

    virtual bool write(std::ostream& stm) const = 0;

    static std::shared_ptr<ValueImpl> read(std::istream& stm)
    {
        char ch = 0;
        while(EOF != (ch = stm.peek()))
        {
            // Ignore whitespace and ','
            if(iswspace(ch) || ',' == ch)
            {
                stm.read(&ch, 1);
                continue;
            }

            switch(ch)
            {
            case 'n':   // Null
                return std::move(NullImpl::read(stm));
            case 't':   // Boolean
            case 'f':
                return std::move(BooleanImpl::read(stm));
            case '\"':  // String
                return std::move(StringImpl::read(stm));
            case '[':   // Array
                return std::move(ArrayImpl::read(stm));
            case '{':   // Object
                return std::move(ObjectImpl::read(stm));
            case '+':   // Number
            case '-':
            case '.':
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                return std::move(StringImpl::read(stm));
            default:
                break;
            }
            return std::shared_ptr<ValueImpl>();
        }
    }

protected:
    ValueImpl() {}

private:
    friend class Value;
};

class NullImpl : public ValueImpl
{
public:
    virtual ~NullImpl() {}

protected:
    NullImpl() {}

private:
    virtual bool write(std::ostream& stm) const override
    {
        stm << "null";
        return true;
    }
    
    static std::shared_ptr<ValueImpl> read(std::istream& stm)
    {
        char buf[5] = {0};
        if (stm.read(buf, 4) && 0 == strcmp(buf, "null"))
            return std::shared_ptr<ValueImpl>(new NullImpl());
        return std::shared_ptr<ValueImpl>();
    }

private:
    friend class Value;
    friend class ValueImpl;
};

class BooleanImpl : public ValueImpl
{
public:
    virtual ~BooleanImpl() {}

protected:
    BooleanImpl() : val(false) {}
    explicit BooleanImpl(bool v) : val(v) {}

    inline void set(bool v) { val = v; }
    inline bool get() const { return val; }

private:
    virtual bool write(std::ostream& stm) const override
    {
        stm << (val ? "true" : "false");
        return true;
    }
    
    static std::shared_ptr<ValueImpl> read(std::istream& stm)
    {
        char buf[6] = {0};
        const char ch = stm.peek();
        if ('t' == ch)
        {
            if (stm.read(buf, 4) && 0 == strcmp(buf, "true"))
                return std::shared_ptr<ValueImpl>(new BooleanImpl(true));
        }
        else if ('f' == ch)
        {
            if (stm.read(buf, 5) && 0 == strcmp(buf, "false"))
                return std::shared_ptr<ValueImpl>(new BooleanImpl(false));
        }
        else
        {
            NOTHING;
        }
        return std::shared_ptr<ValueImpl>();
    }

private:
    bool val;
    friend class Value;
    friend class Boolean;
    friend class ValueImpl;
};

class NumberImpl : public ValueImpl
{
public:
    virtual ~NumberImpl()
    {
    }

protected:
    NumberImpl()
        : signedNumber(false)
        , floatNumber(false)
        , ull(0)
    {
    }

    explicit NumberImpl(int32_t v)
        : signedNumber(v < 0 ? true : false)
        , floatNumber(false)
        , l(v)
    {
    }

    explicit NumberImpl(uint32_t v)
        : signedNumber(false)
        , floatNumber(false)
        , ul(v)
    {
    }

    explicit NumberImpl(int64_t v)
        : signedNumber(v < 0 ? true : false)
        , floatNumber(false)
        , ll(v)
    {
    }

    explicit NumberImpl(uint64_t v)
        : signedNumber(false)
        , floatNumber(false)
        , ull(v)
    {
    }

    explicit NumberImpl(float v)
        : signedNumber(v < 0 ? true : false)
        , floatNumber(true)
        , f(static_cast<double>(v))
    {
    }
    
    explicit NumberImpl(double v)
        : signedNumber(v < 0 ? true : false)
        , floatNumber(true)
        , f(v)
    {
    }

    inline bool isSigned() const { return signedNumber; }
    inline bool isFloat() const { return floatNumber; }

    inline int32_t toInt32() const { return isFloat() ? static_cast<int32_t>(f) : l; }
    inline uint32_t toUint32() const { return isFloat() ? static_cast<uint32_t>(f) : ul; }
    inline int64_t toInt64() const { return isFloat() ? static_cast<int32_t>(f) : ll; }
    inline uint64_t toUint64() const { return isFloat() ? static_cast<uint32_t>(f) : ull; }
    inline double toDecimal() const { return isFloat() ? f : static_cast<double>(ll*1.0); }
    
    inline operator int32_t() const { toInt32(); }
    inline operator uint32_t() const { toUint32(); }
    inline operator int64_t() const { toInt64(); }
    inline operator uint64_t() const { toUint64(); }
    inline operator float() const { static_cast<float>(toDecimal()); }
    inline operator double() const { toDecimal(); }
    
private:
    virtual bool write(std::ostream& stm) const override
    {
        if (floatNumber)
            stm << f;
        else
            stm << ll;
        return true;
    }
    
    static std::shared_ptr<ValueImpl> read(std::istream& stm)
    {
        bool signPresent = false;
        bool dotPresent = false;
        bool exponentPresent = false;
        std::string s;
        char ch = EOF;
        while(!stm.eof() && EOF != (ch = stm.peek()))
        {
            if(ch == '+' || ch == '-')
            {
                if(signPresent) // Sign already present? something is wrong
                    break;
                signPresent = true;
                stm.read(&ch, 1);
                s.append(&ch, 1);
                continue;
            }
            
            if(ch == '.')
            {
                if(dotPresent) // Dot already present? something is wrong
                    break;
                dotPresent = true;
                stm.read(&ch, 1);
                s.append(&ch, 1);
                continue;
            }
            
            if(ch == 'e' || ch == 'E')
            {
                if(exponentPresent) // Exponent already present? something is wrong
                    break;
                exponentPresent = true;
                stm.read(&ch, 1);
                s.append(&ch, 1);
                continue;
            }

            if(ch >= '0' && ch <= '9')
            {
                stm.read(&ch, 1);
                s.append(&ch, 1);
                continue;
            }

            // Not a valid character? stop
            break;
        }

        if(dotPresent || exponentPresent)
            return std::shared_ptr<NumberImpl>(new NumberImpl(std::strtof(s.c_str(), nullptr)));
        else
            return std::shared_ptr<NumberImpl>(new NumberImpl(std::strtoll(s.c_str(), nullptr, 10)));
    }

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
    friend class Value;
    friend class Number;
    friend class ValueImpl;
};

class StringImpl : public ValueImpl
{
public:
    virtual ~StringImpl() {}

protected:
    explicit StringImpl(const std::string& s, bool escaped)
        : val(escaped ? Utils::unescape(s) : s)
    {
    }

    explicit StringImpl(const std::wstring& s, bool escaped)
        : val(escaped ? Utils::unescape(Utils::toUtf8(s)) : Utils::toUtf8(s))
    {
    }

    inline const std::string& get() const { return val; }
    inline void set(const std::string& s, bool escaped) { val = escaped ? Utils::unescape(s) : s; }
    inline void set(const std::wstring& s, bool escaped) { val = escaped ? Utils::unescape(Utils::toUtf8(s)) : Utils::toUtf8(s); }

    virtual bool write(std::ostream& stm) const override
    {
        stm << "\"" << Utils::escape(val) << "\"";
    }
    
    // The input must be escaped
    static std::shared_ptr<ValueImpl> read(std::istream& stm)
    {
    }

private:
    std::string val;
    friend class Value;
    friend class String;
    friend class ValueImpl;
};

class ArrayImpl : public ValueImpl
{
public:
    virtual ~ArrayImpl() {}

protected:
    ArrayImpl()
        : ValueImpl()
    {
    }
    
    typedef std::vector<std::shared_ptr<ValueImpl>>::iterator       iterator;
    typedef std::vector<std::shared_ptr<ValueImpl>>::const_iterator const_iterator;
    typedef std::shared_ptr<ValueImpl>                              value_type;

    inline bool empty() const { return vals.empty(); }
    inline void clear() { vals.clear(); }
    
    void push_back(std::shared_ptr<ValueImpl> pv)
    {
        if(pv != nullptr)
            vals.push_back(pv);
    }

    std::shared_ptr<ValueImpl> operator [] (size_t index)
    {
        return (index < vals.size()) ? vals[index] : std::shared_ptr<ValueImpl>();
    }

    std::shared_ptr<ValueImpl> operator [] (size_t index) const
    {
        return (index < vals.size()) ? vals[index] : std::shared_ptr<ValueImpl>();
    }

private:
    virtual bool write(std::ostream& stm) const override
    {
        bool firstItem = true;
        stm << "[";
        for(const std::shared_ptr<ValueImpl>& sp : vals)
        {
            if(firstItem)
                firstItem = false;
            else
                stm << ",";
            sp->write(stm);
        }
        stm << "]";
    }
    
    static std::shared_ptr<ValueImpl> read(std::istream& stm)
    {
    }

private:
    std::vector<std::shared_ptr<ValueImpl>> vals;
    friend class Value;
    friend class Array;
    friend class ValueImpl;
};

class ObjectImpl : public ValueImpl
{
public:
    virtual ~ObjectImpl() {}

protected:
    ObjectImpl(bool ko)
        : ValueImpl(), keepOrder(ko)
    {
    }

    typedef std::vector<std::pair<std::string, std::shared_ptr<ValueImpl>>>::iterator       iterator;
    typedef std::vector<std::pair<std::string, std::shared_ptr<ValueImpl>>>::const_iterator const_iterator;
    typedef std::pair<std::string, std::shared_ptr<ValueImpl>>                              value_type;

    inline bool keepOriginalOrder() const { return keepOrder; }
    inline bool empty() const { return vals.empty(); }
    inline void clear() { vals.clear(); }

    void set(const std::string& key, std::shared_ptr<ValueImpl> pv)
    {
        if(key.empty() || pv == nullptr)
            return;
        
        if(keepOrder)
        {
            auto pos = std::find_if(vals.begin(), vals.end(), [&](const value_type& item)->bool{
                return (0 == _stricmp(key.c_str(), item.first.c_str()));
            });
            if(pos == vals.end())
            {
                vals.push_back(std::pair<std::string, std::shared_ptr<ValueImpl>>(key, pv));
            }
            else
            {
                (*pos).second = pv;
            }
        }
        else
        {
            std::pair<std::string, std::shared_ptr<ValueImpl>> keyPair(key, std::shared_ptr<ValueImpl>());
            auto pos = std::lower_bound(vals.begin(), vals.end(), keyPair, [](const value_type& v1, const value_type& v2)->bool{
                return (_stricmp(v1.first.c_str(), v2.first.c_str()) < 0);
            });
            if(pos == vals.end())
            {
                vals.push_back(std::pair<std::string, std::shared_ptr<ValueImpl>>(key, pv));
            }
            else
            {
                if(0 == _stricmp(key.c_str(), (*pos).first.c_str()))
                {
                    (*pos).second = pv;
                }
                else
                {
                    vals.insert(pos, std::pair<std::string, std::shared_ptr<ValueImpl>>(key, pv));
                }
            }
        }
    }

    std::shared_ptr<ValueImpl> get(const std::string& key)
    {
        auto pos = std::find_if(vals.begin(), vals.end(), [&](const value_type& item)->bool{
            return (0 == _stricmp(key.c_str(), item.first.c_str()));
        });
        return (pos == vals.end()) ? std::shared_ptr<ValueImpl>() : (*pos).second;
    }

    std::shared_ptr<ValueImpl> get(const std::string& key) const
    {
        auto pos = std::find_if(vals.begin(), vals.end(), [&](const value_type& item)->bool{
            return (0 == _stricmp(key.c_str(), item.first.c_str()));
        });
        return (pos == vals.end()) ? std::shared_ptr<ValueImpl>() : (*pos).second;
    }

    std::shared_ptr<ValueImpl> operator [] (const std::string& key)
    {
        return get(key);
    }

    std::shared_ptr<ValueImpl> operator [] (const std::string& key) const
    {
        return get(key);
    }

    bool remove(const std::string& key)
    {
        auto pos = std::find_if(vals.begin(), vals.end(), [&](const value_type& item)->bool{
            return (0 == _stricmp(key.c_str(), item.first.c_str()));
        });
        if(pos == vals.end())
            return false;
        vals.erase(pos);
        return true;
    }
    
private:
    virtual bool write(std::ostream& stm) const override
    {
        bool firstItem = true;
        stm << "{";
        for(const std::pair<std::string, std::shared_ptr<ValueImpl>>& item : vals)
        {
            if(firstItem)
                firstItem = false;
            else
                stm << ",";
            stm << "\"";
            stm << Utils::escape(item.first);
            stm << "\":";
            item.second->write(stm);
        }
        stm << "}";
    }
    
    static std::shared_ptr<ValueImpl> read(std::istream& stm)
    {
    }

private:
    bool keepOrder;
    std::vector<std::pair<std::string, std::shared_ptr<ValueImpl>>> vals;
    friend class Value;
    friend class Object;
    friend class ValueImpl;
};

}   // namespace Impl




}   // namespace JSONX


#endif
