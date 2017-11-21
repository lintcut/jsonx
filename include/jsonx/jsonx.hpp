#pragma once
#ifndef JSONX_H
#define JSONX_H

#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <cctype>
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

namespace Utils {

    __forceinline std::string toUtf8(const std::string& s)
    {
        return s;
    }

    __forceinline std::string toUtf8(const std::wstring& s)
    {
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> convert;
        return convert.to_bytes(s);    
    }

    __forceinline std::wstring toUtf16(const std::string& s)
    {
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> convert;
        return convert.from_bytes(s); 
    }

    template<typename T> T toLower(T c) { return (c >= 'A' && c <= 'Z') ? (c + 0x20) : c; }
    template<typename T> T toUpper(T c) { return (c >= 'a' && c <= 'z') ? (c - 0x20) : c; }

    template<typename T>
    int compare(T c1, T c2, bool caseInsensitive)
    {
        if (caseInsensitive)
        {
            c1 = toLower<T>(c1);
            c2 = toLower<T>(c2);
        }
        return (c1 == c2) ? 0 : (c1 < c2 ? -1 : 1);
    }

    template<typename T>
    int compare(const T* s1, const T* s2, bool caseInsensitive)
    {
        int result = 0;
        while (0 == result && (*s1 || *s2))
            result = compare<T>(*(s1++), *(s2++), caseInsensitive);
        return result;
    }

    template<typename T>
    bool equal(const T* s1, const T* s2, bool caseInsensitive)
    {
        return (0 == compare<T>(s1, s2, caseInsensitive));
    }

    // JSON escape/unescape rules:
    //  https://tools.ietf.org/html/rfc7159#page-8
    __forceinline std::string escape(const std::string& s)
    {
        std::string s2;
        const char* pos = s.c_str();
        while(*pos)
        {
            switch(*pos)
            {
            case '\"':
                s2.append("\\\"");
                break;
            case '\\':
                s2.append("\\\\");
                break;
            case '/':
                s2.append("\\/");
                break;
            case '\b':
                s2.append("\\b");
                break;
            case '\f':
                s2.append("\\f");
                break;
            case '\n':
                s2.append("\\n");
                break;
            case '\r':
                s2.append("\\r");
                break;
            case '\t':
                s2.append("\\t");
                break;
            default:
                s2.append(pos, 1);
                break;
            }
            ++pos;
        }
        return std::move(s2);
    }

    __forceinline std::string unescape(const std::string& s)
    {
        std::string s2;
        const char* pos = s.c_str();
        while(*pos)
        {
            if(*pos == '\\')
            {
                // escaped character
                ++pos;
                if (*pos == '\"')
                {
                    s2.append("\"");
                    ++pos;
                }
                else if(*pos == '\\')
                {
                    s2.append("\\");
                    ++pos;
                }
                else if (*pos == '/')
                {
                    s2.append("/");
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
                else if (*pos == 'n')
                {
                    s2.append("\n");
                    ++pos;
                }
                else if (*pos == 'r')
                {
                    s2.append("\r");
                    ++pos;
                }
                else if (*pos == 't')
                {
                    s2.append("\t");
                    ++pos;
                }
                else if (*pos == 'u')
                {
                    ++pos;
                    // Next four digits must be hex
                    if(std::isxdigit((int)pos[0]) && std::isxdigit((int)pos[1]) && std::isxdigit((int)pos[2]) && std::isxdigit((int)pos[3]))
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


typedef enum ValueType {
    JsonUnknown = 0,
    JsonNull,
    JsonBoolean,
    JsonNumber,
    JsonString,
    JsonArray,
    JsonObject
} ValueType;

typedef enum JsonError {
    JESuccess = 0,
    JEBadAlloc,
    JEMismatchValueType,
    JEUnexpectedChar,
    JEUnexpectedEnd,
    JEMissingColon
} JsonError;

class SerializeConfig
{
public:
    SerializeConfig()
        : wellFormatted(false)
        , indentSize(0)
        , lineEnding("\n")
    {
    }
    SerializeConfig(bool formatted, const char* eol)
        : wellFormatted(false)
        , indentSize(0)
        , lineEnding(eol)
    {
    }
    ~SerializeConfig()
    {
    }

    inline bool isWellFormatted() const { return wellFormatted; }
    inline const std::vector<char>& getIndent() const { return indent; }
    inline size_t getIndentSize() const { return indentSize; }
    inline void indentInc()
    {
        if (wellFormatted)
        {
            indentSize += 4;
            if (indentSize > indent.size())
            {
                // Always round to 64
                indent.resize(((indent.size() + 63) / 64) * 64, ' ');
            }
        }
    }
    inline void indentDec()
    {
        if (wellFormatted)
        {
            indentSize = (indentSize >= 4) ? (indentSize - 4) : 0;
        }
    }
    inline const std::string& getLineEnding() const { return lineEnding; }

private:
    bool wellFormatted;
    std::vector<char> indent;
    size_t indentSize;
    const std::string lineEnding;
};

namespace IMPLEMENT {

class ValueBase
{
public:
    virtual ~ValueBase() {}

    virtual bool isNull() const { return false; }
    virtual bool isBoolean() const { return false; }
    virtual bool isNumber() const { return false; }
    virtual bool isString() const { return false; }
    virtual bool isObject() const { return false; }
    virtual bool isArray() const { return false; }

    virtual std::string serialize(SerializeConfig* config) const = 0;

protected:
    ValueBase() {}
};

class ValueNull : public ValueBase
{
public:
    virtual ~ValueNull() {}
    virtual bool isNull() const { return true; }
    virtual std::string serialize(SerializeConfig* config) const { return "null"; }

    static ValueNull* create() { return new ValueNull(); }

private:
    ValueNull() {}
};

class ValueBoolean : public ValueBase
{
public:
    virtual ~ValueBoolean() {}
    virtual bool isBoolean() const { return true; }
    virtual std::string serialize(SerializeConfig* config) const { return val ? "true" : "false"; }

    static ValueBoolean* create(bool v) { return new ValueBoolean(v); }

    operator bool() const { return val; }
    inline bool get() const { return val; }
    inline void set(bool v) { val = v; }

private:
    explicit ValueBoolean(bool v)
        : ValueBase()
        , val(v)
    {
    }

    bool val;
};

class ValueNumber : public ValueBase
{
public:
    virtual ~ValueNumber() {}
    virtual bool isNumber() const { return true; }
    virtual std::string serialize(SerializeConfig* config) const { return valDecimal ? std::to_string(d) : std::to_string(n); }

    static ValueNumber* create(int32_t v) { return new ValueNumber(v); }
    static ValueNumber* create(int64_t v) { return new ValueNumber(v); }
    static ValueNumber* create(uint32_t v) { return new ValueNumber(v); }
    static ValueNumber* create(uint64_t v) { return new ValueNumber(v); }
    static ValueNumber* create(float v) { return new ValueNumber(v); }
    static ValueNumber* create(double v) { return new ValueNumber(v); }

    inline bool isSigned() const { return valSigned; }
    inline bool isDecimal() const { return valDecimal; }
    inline bool isInteger() const { return !valDecimal; }

    inline int32_t toInt32() const { return valDecimal ? static_cast<int32_t>(d) : static_cast<int32_t>(n); }
    inline int64_t toInt64() const { return valDecimal ? static_cast<int64_t>(d) : n; }
    inline uint32_t toUint32() const { return valDecimal ? static_cast<uint32_t>(d) : static_cast<uint32_t>(u); }
    inline uint64_t toUint64() const { return valDecimal ? static_cast<uint64_t>(d) : u; }
    inline double toDecimal() const { return valDecimal ? d : (n*1.0); }

    inline void set(int32_t v) { n = v; valSigned = (v < 0); valDecimal = false; }
    inline void set(int64_t v) { n = v; valSigned = (v < 0); valDecimal = false; }
    inline void set(uint32_t v) { u = v; valSigned = false; valDecimal = false; }
    inline void set(uint64_t v) { u = v; valSigned = false; valDecimal = false; }
    inline void set(float_t v) { d = v; valSigned = (v < 0); valDecimal = true; }
    inline void set(double_t v) { d = v; valSigned = (v < 0); valDecimal = true; }

private:
    explicit ValueNumber(int32_t v)
        : ValueBase()
        , n(v)
        , valSigned(v < 0)
        , valDecimal(false)
    {
    }
    explicit ValueNumber(int64_t v)
        : ValueBase()
        , n(v)
        , valSigned(v < 0)
        , valDecimal(false)
    {
    }
    explicit ValueNumber(uint32_t v)
        : ValueBase()
        , u(v)
        , valSigned(false)
        , valDecimal(false)
    {
    }
    explicit ValueNumber(uint64_t v)
        : ValueBase()
        , u(v)
        , valSigned(false)
        , valDecimal(false)
    {
    }
    explicit ValueNumber(float v)
        : ValueBase()
        , d(v)
        , valSigned(v < 0)
        , valDecimal(true)
    {
    }
    explicit ValueNumber(double v)
        : ValueBase()
        , d(v)
        , valSigned(v < 0)
        , valDecimal(true)
    {
    }

    bool valSigned;
    bool valDecimal;
    union {
        int64_t n;
        uint64_t u;
        double_t d;
    };
};

class ValueString : public ValueBase
{
public:
    virtual ~ValueString() {}
    virtual bool isString() const { return true; }
    virtual std::string serialize(SerializeConfig* config) const
    {
        std::string s("\"");
        s.append(Utils::escape(val));
        s.append("\"");
        return s;
    }

    static ValueString* create(const std::string& s, bool escaped) { return new ValueString(s, escaped); }
    static ValueString* create(const std::wstring& s, bool escaped) { return new ValueString(s, escaped); }

    inline bool empty() const { return val.empty(); }
    inline void clear() { val.clear(); }
    inline const std::string& get() const { return val; }
    inline std::wstring getw() const { return Utils::toUtf16(val); }

    void set(const std::string& s, bool escaped)
    {
        val = escaped ? s : Utils::escape(s);
    }
    void set(const std::wstring& s, bool escaped)
    {
        val = escaped ? Utils::toUtf8(s) : Utils::escape(Utils::toUtf8(s));
    }

private:
    explicit ValueString(const std::string& s, bool escaped)
        : val(escaped ? Utils::unescape(s) : s)
    {
    }

    explicit ValueString(const std::wstring& s, bool escaped)
        : val(escaped ? Utils::unescape(Utils::toUtf8(s)) : Utils::toUtf8(s))
    {
    }

    std::string val;
};

class ValueObject : public ValueBase
{
public:
    virtual ~ValueObject() {}
    virtual bool isObject() const { return true; }
    virtual std::string serialize(SerializeConfig* config) const
    {
        std::string s("{");
        if (config && config->isWellFormatted())
            config->indentInc();
        std::for_each(begin(), end(), [&](const value_type& item) {
            if (s.length() > 1)
            {
                s.append(",");
                if (config && config->isWellFormatted())
                    s.append(config->getLineEnding());
            }

            // Add indent
            if (config && config->isWellFormatted() && config->getIndentSize() != 0)
                s.append(config->getIndent().data(), config->getIndent().data() + config->getIndentSize());

            // Key
            s.append("\"");
            s.append(Utils::escape(item.first));
            s.append("\":");
            // Value
            s.append(item.second->serialize(config));
        });
        if (config && config->isWellFormatted())
        {
            config->indentDec();
            s.append(config->getLineEnding());
            if (config->getIndentSize() != 0)
                s.append(config->getIndent().data(), config->getIndent().data() + config->getIndentSize());
        }
        s.append("}");
        return s;
    }

    static ValueObject* create(bool ko = true) { return new ValueObject(ko); }

    typedef std::pair<std::string, std::shared_ptr<ValueBase>> value_type;
    typedef std::vector<value_type>::iterator iterator;
    typedef std::vector<value_type>::const_iterator const_iterator;

    inline bool keepInitOrder() const { return keepOrder; }
    inline bool empty() const { return vals.empty(); }
    inline void clear() { vals.clear(); }
    inline size_t size() const { return vals.size(); }
    inline iterator begin() { return vals.begin(); }
    inline const_iterator begin() const { return vals.begin(); }
    inline iterator end() { return vals.end(); }
    inline const_iterator end() const { return vals.end(); }

    std::shared_ptr<ValueBase> get(const std::wstring& key) const
    {
        return get(Utils::toUtf8(key));
    }

    std::shared_ptr<ValueBase> get(const std::string& key) const
    {
        const_iterator pos = find(key);
        return (pos != vals.end()) ? (*pos).second : std::shared_ptr<ValueBase>(nullptr);
    }

    std::shared_ptr<ValueBase> set(const std::string& key, std::shared_ptr<ValueBase> sp)
    {
        iterator pos = find(key);
        if (pos != vals.end())
        {
            (*pos).second = sp;
        }
        else
        {
            if (keepOrder)
            {
                vals.push_back(value_type(key, sp));
            }
            else
            {
                pos = std::lower_bound(vals.begin(), vals.end(), key, [](const value_type& val, const std::string& key)->bool {
                    return (0 > Utils::compare<char>(val.first.c_str(), key.c_str(), true));
                });
                vals.insert(pos, value_type(key, sp));
            }
        }
        return sp;
    }

    std::shared_ptr<ValueBase> set(const std::string& key) { return set(key, std::shared_ptr<ValueBase>(ValueNull::create())); }
    std::shared_ptr<ValueBase> set(const std::string& key, bool v) { return set(key, std::shared_ptr<ValueBase>(ValueBoolean::create(v))); }
    std::shared_ptr<ValueBase> set(const std::string& key, int32_t v) { return set(key, std::shared_ptr<ValueBase>(ValueNumber::create(v))); }
    std::shared_ptr<ValueBase> set(const std::string& key, int64_t v) { return set(key, std::shared_ptr<ValueBase>(ValueNumber::create(v))); }
    std::shared_ptr<ValueBase> set(const std::string& key, uint32_t v) { return set(key, std::shared_ptr<ValueBase>(ValueNumber::create(v))); }
    std::shared_ptr<ValueBase> set(const std::string& key, uint64_t v) { return set(key, std::shared_ptr<ValueBase>(ValueNumber::create(v))); }
    std::shared_ptr<ValueBase> set(const std::string& key, float v) { return set(key, std::shared_ptr<ValueBase>(ValueNumber::create(v))); }
    std::shared_ptr<ValueBase> set(const std::string& key, double v) { return set(key, std::shared_ptr<ValueBase>(ValueNumber::create(v))); }
    std::shared_ptr<ValueBase> set(const std::string& key, const std::string& v) { return set(key, std::shared_ptr<ValueBase>(ValueString::create(v, false))); }
    std::shared_ptr<ValueBase> set(const std::string& key, const std::wstring& v) { return set(key, std::shared_ptr<ValueBase>(ValueString::create(v, false))); }

private:
    iterator find(const std::string& key)
    {
        iterator pos = std::find_if(vals.begin(), vals.end(), [&](const value_type& item)->bool {
            return Utils::equal<char>(item.first.c_str(), key.c_str(), true);
        });
        return pos;
    }

    const_iterator find(const std::string& key) const
    {
        const_iterator pos = std::find_if(vals.begin(), vals.end(), [&](const value_type& item)->bool {
            return Utils::equal<char>(item.first.c_str(), key.c_str(), true);
        });
        return pos;
    }


private:
    explicit ValueObject(bool ko) : keepOrder(ko) {}

    bool keepOrder;
    std::vector<value_type> vals;
};

class ValueArray : public ValueBase
{
public:
    virtual ~ValueArray() {}
    virtual bool isArray() const { return true; }
    virtual std::string serialize(SerializeConfig* config) const
    {
        std::string s("[");
        if (config && config->isWellFormatted())
            config->indentInc();
        std::for_each(begin(), end(), [&](const value_type& item) {
            if (s.length() > 1)
            {
                s.append(",");
                if (config && config->isWellFormatted())
                    s.append(config->getLineEnding());
            }

            // Add indent
            if (config && config->isWellFormatted() && config->getIndentSize() != 0)
                s.append(config->getIndent().data(), config->getIndent().data() + config->getIndentSize());

            // Value
            s.append(item->serialize(config));
        });
        if (config && config->isWellFormatted())
        {
            config->indentDec();
            s.append(config->getLineEnding());
            if (config->getIndentSize() != 0)
                s.append(config->getIndent().data(), config->getIndent().data() + config->getIndentSize());
        }
        s.append("]");
        return s;
    }

    static ValueArray* create() { return new ValueArray(); }

    typedef std::shared_ptr<ValueBase> value_type;
    typedef std::vector<value_type>::iterator iterator;
    typedef std::vector<value_type>::const_iterator const_iterator;

    inline bool empty() const { return vals.empty(); }
    inline void clear() { vals.clear(); }
    inline size_t size() const { return vals.size(); }
    inline iterator begin() { return vals.begin(); }
    inline const_iterator begin() const { return vals.begin(); }
    inline iterator end() { return vals.end(); }
    inline const_iterator end() const { return vals.end(); }

    std::shared_ptr<ValueBase> get(size_t index) const
    {
        return (index < vals.size()) ? vals[index] : std::shared_ptr<ValueBase>();
    }

    std::shared_ptr<ValueBase> push_back(std::shared_ptr<ValueBase> sp) { vals.push_back(sp); return sp; }
    std::shared_ptr<ValueBase> push_back(bool v) { return push_back(std::shared_ptr<ValueBase>(ValueBoolean::create(v))); }
    std::shared_ptr<ValueBase> push_back(int32_t v) { return push_back(std::shared_ptr<ValueBase>(ValueNumber::create(v))); }
    std::shared_ptr<ValueBase> push_back(int64_t v) { return push_back(std::shared_ptr<ValueBase>(ValueNumber::create(v))); }
    std::shared_ptr<ValueBase> push_back(uint32_t v) { return push_back(std::shared_ptr<ValueBase>(ValueNumber::create(v))); }
    std::shared_ptr<ValueBase> push_back(uint64_t v) { return push_back(std::shared_ptr<ValueBase>(ValueNumber::create(v))); }
    std::shared_ptr<ValueBase> push_back(float v) { return push_back(std::shared_ptr<ValueBase>(ValueNumber::create(v))); }
    std::shared_ptr<ValueBase> push_back(double v) { return push_back(std::shared_ptr<ValueBase>(ValueNumber::create(v))); }
    std::shared_ptr<ValueBase> push_back(const std::string& v) { return push_back(std::shared_ptr<ValueBase>(ValueString::create(v, false))); }
    std::shared_ptr<ValueBase> push_back(const std::wstring& v) { return push_back(std::shared_ptr<ValueBase>(ValueString::create(v, false))); }

private:
    ValueArray() {}
    std::vector<std::shared_ptr<ValueBase>> vals;
};

class Parser
{
public:
    Parser(std::istream& s)
        : stm(s), pos(0), error(JESuccess)
    {
    }
    ~Parser() {}

    inline size_t getPos() const { return pos; }
    inline JsonError getError() const { return error; }
    inline bool failed() const { return (0 != error); }
    virtual void reset() { stm.seekg(0); }

    ValueType checkValueType()
    {
        char c = peekNextNotSpace();
        switch (c)
        {
        case 'n':
        case 'N':
            return JsonNull;
        case 't':
        case 'T':
        case 'f':
        case 'F':
            return JsonBoolean;
        case '-':
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
            return JsonNumber;
        case '\"':
            return JsonString;
        case '{':
            return JsonObject;
        case '[':
            return JsonArray;
        default:
            break;
        }
        return JsonUnknown;
    }

    IMPLEMENT::ValueBase* readValue()
    {
        const ValueType type = checkValueType();

        switch (type)
        {
        case JsonNull:
            return readValueNull();
        case JsonBoolean:
            return readValueBoolean();
        case JsonNumber:
            return readValueNumber();
        case JsonString:
            return readValueString();
        case JsonObject:
            return readValueObject();
        case JsonArray:
            return readValueArray();
        default:
            break;
        }

        error = JEUnexpectedChar;
        return nullptr;
    }

#ifndef _DEBUG
protected:
#endif
    char peekNext() const
    {
        return stm.eof() ? -1 : stm.peek();
    }

    char readNext()
    {
        char ch = -1;
        if(!stm.eof())
        {
            stm.read(&ch, 1);
            ++pos;
        }
        return ch;
    }

    char peekNextNotSpace()
    {
        char ch = peekNext();
        while (!stm.eof() && std::isspace(ch))
        {
            readNext();
            ch = peekNext();
        }
        return ch;
    }

    IMPLEMENT::ValueNull* readValueNull()
    {
        // null
        do {
            char c = readNext();
            if (c != 'n' && c != 'N')
            {
                error = JEMismatchValueType;
                break;
            }
            c = readNext();
            if (c != 'u')
            {
                error = JEUnexpectedChar;
                break;
            }
            c = readNext();
            if (c != 'l')
            {
                error = JEUnexpectedChar;
                break;
            }
            c = readNext();
            if (c != 'l')
            {
                error = JEUnexpectedChar;
                break;
            }
        } while (false);
        return failed() ? nullptr : IMPLEMENT::ValueNull::create();
    }

    IMPLEMENT::ValueBoolean* readValueBoolean()
    {
        // true/false
        bool result = false;
        do {
            char c = readNext();
            if (c == 't' || c == 'T')
            {
                c = readNext();
                if (c != 'r')
                {
                    error = JEUnexpectedChar;
                    break;
                }
                c = readNext();
                if (c != 'u')
                {
                    error = JEUnexpectedChar;
                    break;
                }
                c = readNext();
                if (c != 'e')
                {
                    error = JEUnexpectedChar;
                    break;
                }
                result = true;
            }
            else if (c == 'f' || c == 'F')
            {
                c = readNext();
                if (c != 'a')
                {
                    error = JEUnexpectedChar;
                    break;
                }
                c = readNext();
                if (c != 'l')
                {
                    error = JEUnexpectedChar;
                    break;
                }
                c = readNext();
                if (c != 's')
                {
                    error = JEUnexpectedChar;
                    break;
                }
                c = readNext();
                if (c != 'e')
                {
                    error = JEUnexpectedChar;
                    break;
                }
                result = false;
            }
            else
            {
                error = JEMismatchValueType;
            }
        } while (false);

        return failed() ? nullptr : IMPLEMENT::ValueBoolean::create(result);
    }

    IMPLEMENT::ValueNumber* readValueNumber()
    {
        // Number = [minus] int [frac] [exp]
        // {
        //    decimal - point = %x2E; .
        //    digit1 - 9 = %x31 - 39; 1 - 9
        //    e = %x65 / %x45; e E
        //    exp = e[minus / plus] 1 * DIGIT
        //    frac = decimal - point 1 * DIGIT
        //    int = zero / (digit1 - 9 * DIGIT)
        //    minus = %x2D; -
        //    plus = %x2B; +
        //    zero = %x30; 0
        // }

        char c = peekNextNotSpace();
        bool dotFlag = false;
        bool eFlag = false;
        bool signFlag = false;

        std::string s;

        if (c == '-')
        {
            signFlag = true;
            s.append(&c, &c + 1);
            readNext();
            c = peekNext();
        }

        do {

            if (c == '-')
            {
                error = JEUnexpectedChar;
                break;
            }
            else if (c == '.')
            {
                if (dotFlag)
                {
                    error = JEUnexpectedChar;
                    break;
                }
                else
                {
                    dotFlag = true;
                    s.append(&c, &c + 1);
                    readNext();
                    c = peekNext();
                }
            }
            else if (c == 'e' || c == 'E')
            {
                if (eFlag)
                {
                    error = JEUnexpectedChar;
                    break;
                }
                
                eFlag = true;
                s.append(&c, &c + 1);
                readNext();
                c = peekNext();
                if (c == '+' || c == '-')
                {
                    s.append(&c, &c + 1);
                    readNext();
                    c = peekNext();
                }
            }
            else
            {
                if (c <'0' || c > '9')
                {
                    break;
                }
                s.append(&c, &c + 1);
                readNext();
                c = peekNext();
            }

        } while (!stm.eof());

        if (failed())
            return nullptr;

        IMPLEMENT::ValueNumber* pNumber = nullptr;

        if (dotFlag || eFlag)
        {
            // Decimal
            pNumber = IMPLEMENT::ValueNumber::create(std::stod(s));
        }
        else
        {
            // Integer
            pNumber = IMPLEMENT::ValueNumber::create(std::stoll(s));
        }

        if (pNumber == nullptr)
            error = JEBadAlloc;

        return pNumber;
    }

    std::string readEscapedString()
    {
        char c = readNext();
        if (c != '\"')
        {
            error = JEMismatchValueType;
            return nullptr;
        }

        std::string s;

        do {

            c = readNext();
            if (stm.eof())
            {
                error = JEUnexpectedEnd;
                break;
            }

            // Finish
            if ('\"' == c)
                break;

            s.append(&c, &c + 1);

            if ('\\' == c)
            {
                c = readNext();
                if (stm.eof())
                {
                    error = JEUnexpectedEnd;
                    break;
                }
                s.append(&c, &c + 1);
            }

        } while (true);

        return s;
    }

    IMPLEMENT::ValueString* readValueString()
    {
        const std::string& s = readEscapedString();
        return failed() ? nullptr : IMPLEMENT::ValueString::create(s, true);
    }

    IMPLEMENT::ValueObject* readValueObject()
    {
        char c = readNext();
        if (c != '{')
        {
            error = JEMismatchValueType;
            return nullptr;
        }

        IMPLEMENT::ValueObject* pObject = IMPLEMENT::ValueObject::create();
        if(nullptr == pObject)
        {
            error = JEBadAlloc;
            return nullptr;
        }

        do {

            c = peekNextNotSpace();
            if(stm.eof())
            {
                error = JEUnexpectedEnd;
                break;
            }

            // Done
            if (c == '}')
            {
                readNext();
                break;
            }

            if (c == ',')
            {
                readNext();
                continue;
            }

            if('\"' != c)
            {
                error = JEUnexpectedChar;
                break;
            }

            // Read key
            const std::string& key = readEscapedString();
            if (failed())
                break;

            // Read colon
            c = peekNextNotSpace();
            if (failed())
                break;
            if (c != ':')
            {
                error = JEMissingColon;
                break;
            }
            // Skip it
            readNext();

            // Read Value
            IMPLEMENT::ValueBase* value = readValue();
            if (failed() || value == nullptr)
                break;

            // Insert new child item
            pObject->set(Utils::unescape(key), std::shared_ptr<IMPLEMENT::ValueBase>(value));

        } while (true);

        if (failed())
        {
            delete pObject;
            pObject = nullptr;
        }

        return pObject;
    }

    IMPLEMENT::ValueArray* readValueArray()
    {
        char c = readNext();
        if (c != '[')
        {
            error = JEMismatchValueType;
            return nullptr;
        }

        IMPLEMENT::ValueArray* pArray = IMPLEMENT::ValueArray::create();
        if (nullptr == pArray)
        {
            error = JEBadAlloc;
            return nullptr;
        }

        do {

            c = peekNextNotSpace();
            if (stm.eof())
            {
                error = JEUnexpectedEnd;
                break;
            }

            // Done
            if (c == ']')
            {
                readNext();
                break;
            }

            if (c == ',')
            {
                readNext();
                continue;
            }

            // Read Value
            IMPLEMENT::ValueBase* value = readValue();
            if (failed() || value == nullptr)
                break;

            // Insert new child item
            pArray->push_back(std::shared_ptr<IMPLEMENT::ValueBase>(value));

        } while (true);

        if (failed())
        {
            delete pArray;
            pArray = nullptr;
        }

        return pArray;
    }

private:
    std::istream& stm;
    size_t pos;
    JsonError error;
};

template<typename T>
class StringParser : public Parser
{
public:
    StringParser(const std::basic_string<T>& s)
        : iss(Utils::toUtf8(s)), Parser(iss)
    {
    }
    virtual ~StringParser()
    {
    }

    virtual void reset(const std::basic_string<T>& s)
    {
        iss.str(Utils::toUtf8(s));
        Parser::reset();
    }

private:
    std::istringstream iss;
};

}   // namespace IMPLEMENT

class ValueFactory
{
public:
    static IMPLEMENT::ValueBase* create(ValueType type)
    {
        switch (type)
        {
        case JsonNull:
            return IMPLEMENT::ValueNull::create();
        case JsonBoolean:
            return IMPLEMENT::ValueBoolean::create(false);
        case JsonNumber:
            return IMPLEMENT::ValueNumber::create(0LL);
        case JsonString:
            return IMPLEMENT::ValueString::create("", false);
        case JsonObject:
            return IMPLEMENT::ValueObject::create(true);
        case JsonArray:
            return IMPLEMENT::ValueArray::create();
        default:
            break;
        }
        return nullptr;
    }

    static IMPLEMENT::ValueNull* createNull()
    {
        IMPLEMENT::ValueNull::create();
    }

    static IMPLEMENT::ValueBoolean* createBoolean(bool v)
    {
        IMPLEMENT::ValueBoolean::create(v);
    }

    static IMPLEMENT::ValueString* createString(const std::string& s, bool escaped)
    {
        IMPLEMENT::ValueString::create(s, escaped);
    }

    static IMPLEMENT::ValueString* createString(const std::wstring& s, bool escaped)
    {
        IMPLEMENT::ValueString::create(s, escaped);
    }

    static IMPLEMENT::ValueObject* createObject(bool keepOrder)
    {
        IMPLEMENT::ValueObject::create(keepOrder);
    }

    static IMPLEMENT::ValueArray* createArray()
    {
        IMPLEMENT::ValueArray::create();
    }
};

class Value
{
public:
    Value() : vp(std::shared_ptr<IMPLEMENT::ValueBase>(IMPLEMENT::ValueNull::create())) {}
    Value(const Value& rhs) : vp(rhs.vp) {}
    explicit Value(bool v) : vp(std::shared_ptr<IMPLEMENT::ValueBase>(IMPLEMENT::ValueBoolean::create(v))) {}
    explicit Value(int32_t v) : vp(std::shared_ptr<IMPLEMENT::ValueBase>(IMPLEMENT::ValueNumber::create(v))) {}
    explicit Value(int64_t v) : vp(std::shared_ptr<IMPLEMENT::ValueBase>(IMPLEMENT::ValueNumber::create(v))) {}
    explicit Value(uint32_t v) : vp(std::shared_ptr<IMPLEMENT::ValueBase>(IMPLEMENT::ValueNumber::create(v))) {}
    explicit Value(uint64_t v) : vp(std::shared_ptr<IMPLEMENT::ValueBase>(IMPLEMENT::ValueNumber::create(v))) {}
    explicit Value(float_t v) : vp(std::shared_ptr<IMPLEMENT::ValueBase>(IMPLEMENT::ValueNumber::create(v))) {}
    explicit Value(double_t v) : vp(std::shared_ptr<IMPLEMENT::ValueBase>(IMPLEMENT::ValueNumber::create(v))) {}
    explicit Value(const std::string& v, bool escaped=false) : vp(std::shared_ptr<IMPLEMENT::ValueBase>(IMPLEMENT::ValueString::create(v, escaped))) {}
    explicit Value(const std::wstring& v, bool escaped=false) : vp(std::shared_ptr<IMPLEMENT::ValueBase>(IMPLEMENT::ValueString::create(v, escaped))) {}
    explicit Value(std::shared_ptr<IMPLEMENT::ValueBase> p) : vp(p) {}
    explicit Value(ValueType vt)
        : vp(std::shared_ptr<IMPLEMENT::ValueBase>(ValueFactory::create(vt)))
    {
    }
    virtual ~Value() {}

    Value& operator = (const Value& rhs)
    {
        if (this != &rhs)
        {
            vp = rhs.vp;
        }
        return *this;
    }

    inline bool valid() const { return (nullptr != vp); }
    inline std::string serialize(SerializeConfig* config = nullptr) { return vp->serialize(config); }

    inline bool isNull() const { return valid() && vp->isNull(); }
    inline bool isBoolean() const { return valid() && vp->isBoolean(); }
    inline bool isNumber() const { return valid() && vp->isNumber(); }
    inline bool isString() const { return valid() && vp->isString(); }
    inline bool isObject() const { return valid() && vp->isObject(); }
    inline bool isArray() const { return valid() && vp->isArray(); }

    inline bool getBoolean() const { return (isBoolean() && dynamic_cast<IMPLEMENT::ValueBoolean*>(vp.get())->get()); }
    inline void set(bool v) { if (isBoolean()) dynamic_cast<IMPLEMENT::ValueBoolean*>(vp.get())->set(v); }

    inline bool isSignedNumber() const { return (isNumber() && dynamic_cast<IMPLEMENT::ValueNumber*>(vp.get())->isSigned()); }
    inline bool isIntegerNumber() const { return (isNumber() && dynamic_cast<IMPLEMENT::ValueNumber*>(vp.get())->isInteger()); }
    inline bool isDecimalNumber() const { return (isNumber() && dynamic_cast<IMPLEMENT::ValueNumber*>(vp.get())->isDecimal()); }
    inline int32_t getInt32() const
    {
        return isNumber() ? dynamic_cast<IMPLEMENT::ValueNumber*>(vp.get())->toInt32() : 0;
    }
    inline int64_t getInt64() const
    {
        return isNumber() ? dynamic_cast<IMPLEMENT::ValueNumber*>(vp.get())->toInt64() : 0;
    }
    inline uint32_t getUint32() const
    {
        return isNumber() ? dynamic_cast<IMPLEMENT::ValueNumber*>(vp.get())->toUint32() : 0;
    }
    inline uint64_t getUint64() const
    {
        return isNumber() ? dynamic_cast<IMPLEMENT::ValueNumber*>(vp.get())->toUint64() : 0;
    }
    inline double_t getDecimal() const
    {
        return isNumber() ? dynamic_cast<IMPLEMENT::ValueNumber*>(vp.get())->toDecimal() : 0.0;
    }

    inline void set(int32_t v) { if (isNumber()) dynamic_cast<IMPLEMENT::ValueNumber*>(vp.get())->set(v); }
    inline void set(int64_t v) { if (isNumber()) dynamic_cast<IMPLEMENT::ValueNumber*>(vp.get())->set(v); }
    inline void set(uint32_t v) { if (isNumber()) dynamic_cast<IMPLEMENT::ValueNumber*>(vp.get())->set(v); }
    inline void set(uint64_t v) { if (isNumber()) dynamic_cast<IMPLEMENT::ValueNumber*>(vp.get())->set(v); }
    inline void set(float_t v) { if (isNumber()) dynamic_cast<IMPLEMENT::ValueNumber*>(vp.get())->set(v); }
    inline void set(double_t v) { if (isNumber()) dynamic_cast<IMPLEMENT::ValueNumber*>(vp.get())->set(v); }

    inline const std::string& getString() const
    {
        return dynamic_cast<IMPLEMENT::ValueString*>(vp.get())->get();
    }
    inline std::wstring getWstring() const
    {
        return dynamic_cast<IMPLEMENT::ValueString*>(vp.get())->getw();
    }

    Value operator [](const std::string& key)
    {
        return isObject() ? Value() : Value(dynamic_cast<IMPLEMENT::ValueObject*>(vp.get())->get(key));
    }

    const Value& operator [](const std::string& key) const
    {
        return isObject() ? Value() : Value(dynamic_cast<IMPLEMENT::ValueObject*>(vp.get())->get(key));
    }

    Value set(const std::string& key, std::shared_ptr<IMPLEMENT::ValueBase> sp)
    {
        return isObject()
            ? Value(dynamic_cast<IMPLEMENT::ValueObject*>(vp.get())->set(key, sp))
            : Value(std::shared_ptr<IMPLEMENT::ValueBase>());
    }
    Value set(const std::string& key, Value& v)
    {
        return isObject()
            ? Value(dynamic_cast<IMPLEMENT::ValueObject*>(vp.get())->set(key, v.getPtr()))
            : Value(std::shared_ptr<IMPLEMENT::ValueBase>());
    }
    Value set(const std::string& key, bool v)
    {
        return isObject()
            ? Value(dynamic_cast<IMPLEMENT::ValueObject*>(vp.get())->set(key, v))
            : Value(std::shared_ptr<IMPLEMENT::ValueBase>());
    }
    Value set(const std::string& key, int32_t v)
    {
        return isObject()
            ? Value(dynamic_cast<IMPLEMENT::ValueObject*>(vp.get())->set(key, v))
            : Value(std::shared_ptr<IMPLEMENT::ValueBase>());
    }
    Value set(const std::string& key, uint32_t v)
    {
        return isObject()
            ? Value(dynamic_cast<IMPLEMENT::ValueObject*>(vp.get())->set(key, v))
            : Value(std::shared_ptr<IMPLEMENT::ValueBase>());
    }
    Value set(const std::string& key, int64_t v)
    {
        return isObject()
            ? Value(dynamic_cast<IMPLEMENT::ValueObject*>(vp.get())->set(key, v))
            : Value(std::shared_ptr<IMPLEMENT::ValueBase>());
    }
    Value set(const std::string& key, uint64_t v)
    {
        return isObject()
            ? Value(dynamic_cast<IMPLEMENT::ValueObject*>(vp.get())->set(key, v))
            : Value(std::shared_ptr<IMPLEMENT::ValueBase>());
    }
    Value set(const std::string& key, double_t v)
    {
        return isObject()
            ? Value(dynamic_cast<IMPLEMENT::ValueObject*>(vp.get())->set(key, v))
            : Value(std::shared_ptr<IMPLEMENT::ValueBase>());
    }
    Value set(const std::string& key, const std::string& v)
    {
        return isObject()
            ? Value(dynamic_cast<IMPLEMENT::ValueObject*>(vp.get())->set(key, v))
            : Value(std::shared_ptr<IMPLEMENT::ValueBase>());
    }
    Value set(const std::string& key, const std::wstring& v)
    {
        return isObject()
            ? Value(dynamic_cast<IMPLEMENT::ValueObject*>(vp.get())->set(key, v))
            : Value(std::shared_ptr<IMPLEMENT::ValueBase>());
    }

    Value operator [](size_t id)
    {
        return isArray() ? Value() : Value(dynamic_cast<IMPLEMENT::ValueArray*>(vp.get())->get(id));
    }

    const Value& operator [](size_t id) const
    {
        return isArray() ? Value() : Value(dynamic_cast<IMPLEMENT::ValueArray*>(vp.get())->get(id));
    }

    Value push_back(std::shared_ptr<IMPLEMENT::ValueBase> sp)
    {
        return isArray()
            ? Value(dynamic_cast<IMPLEMENT::ValueArray*>(vp.get())->push_back(sp))
            : Value(std::shared_ptr<IMPLEMENT::ValueBase>());
    }
    Value push_back(bool v)
    {
        return isArray()
            ? Value(dynamic_cast<IMPLEMENT::ValueArray*>(vp.get())->push_back(v))
            : Value(std::shared_ptr<IMPLEMENT::ValueBase>());
    }
    Value push_back(int32_t v)
    {
        return isArray()
            ? Value(dynamic_cast<IMPLEMENT::ValueArray*>(vp.get())->push_back(v))
            : Value(std::shared_ptr<IMPLEMENT::ValueBase>());
    }
    Value push_back(uint32_t v)
    {
        return isArray()
            ? Value(dynamic_cast<IMPLEMENT::ValueArray*>(vp.get())->push_back(v))
            : Value(std::shared_ptr<IMPLEMENT::ValueBase>());
    }
    Value push_back(int64_t v)
    {
        return isArray()
            ? Value(dynamic_cast<IMPLEMENT::ValueArray*>(vp.get())->push_back(v))
            : Value(std::shared_ptr<IMPLEMENT::ValueBase>());
    }
    Value push_back(uint64_t v)
    {
        return isArray()
            ? Value(dynamic_cast<IMPLEMENT::ValueArray*>(vp.get())->push_back(v))
            : Value(std::shared_ptr<IMPLEMENT::ValueBase>());
    }
    Value push_back(double_t v)
    {
        return isArray()
            ? Value(dynamic_cast<IMPLEMENT::ValueArray*>(vp.get())->push_back(v))
            : Value(std::shared_ptr<IMPLEMENT::ValueBase>());
    }
    Value push_back(const std::string& v)
    {
        return isArray()
            ? Value(dynamic_cast<IMPLEMENT::ValueArray*>(vp.get())->push_back(v))
            : Value(std::shared_ptr<IMPLEMENT::ValueBase>());
    }
    Value push_back(const std::wstring& v)
    {
        return isArray()
            ? Value(dynamic_cast<IMPLEMENT::ValueArray*>(vp.get())->push_back(v))
            : Value(std::shared_ptr<IMPLEMENT::ValueBase>());
    }

protected:
    std::shared_ptr<IMPLEMENT::ValueBase> getPtr() { return vp; }

private:
    std::shared_ptr<IMPLEMENT::ValueBase> vp;
};

}   // namespace JSONX


#endif
