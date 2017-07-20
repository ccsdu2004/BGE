#ifndef BGE_SYSTEM_STRING_H
#define BGE_SYSTEM_STRING_H
#include <BGE/Config.h>
#include <BGE/System/Err.h>
#include <locale>
#include <string>
#include <cmath>
#include <sstream>

namespace bge
{

class BGE_EXPORT_API String
{
public :
    typedef std::basic_string<uint32_t>::iterator       Iterator;
    typedef std::basic_string<uint32_t>::const_iterator ConstIterator;

    static const std::size_t InvalidPos;

    String();
    String(char ch, const std::locale& locale = std::locale());
    String(uint16_t ch);
    String(uint32_t utf);
    String(const char* ansiString, const std::locale& locale = std::locale());
    String(const std::string& ansiString, const std::locale& locale = std::locale());
    String(const wchar_t* string);
    String(const std::wstring& string);
    String(const uint32_t* utf32);
    String(const std::basic_string<uint32_t>& utf32);
    String(const String& copy);

    operator std::string() const;
    operator std::wstring() const;
    std::string toAnsiString(const std::locale& locale = std::locale()) const;
    std::wstring toWideString() const;

    String& operator =(const String& right);
    String& operator +=(const String& right);
    uint32_t operator [](std::size_t index) const;
    uint32_t& operator [](std::size_t index);

    void clear();

    std::size_t size() const;
    int32_t length()const{return size();}
    bool empty() const;

    void erase(std::size_t position, std::size_t count = 1);
    void insert(std::size_t position, const String& str);
	void push_back(uint32_t ch);

    std::size_t find(const String& str, std::size_t start = 0) const;
    const uint32_t* data() const;

    Iterator begin();
    ConstIterator begin() const;
    Iterator end();
    ConstIterator end() const;

    std::basic_string<uint32_t> get() const { return m_string; }
private :
    friend BGE_EXPORT_API bool operator ==(const String& left, const String& right);
    friend BGE_EXPORT_API bool operator <(const String& left, const String& right);
    std::basic_string<uint32_t> m_string; ///< Internal string of UTF-32 characters
};

BGE_EXPORT_API bool operator ==(const String& left, const String& right);
BGE_EXPORT_API bool operator !=(const String& left, const String& right);
BGE_EXPORT_API bool operator <(const String& left, const String& right);
BGE_EXPORT_API bool operator >(const String& left, const String& right);
BGE_EXPORT_API bool operator <=(const String& left, const String& right);
BGE_EXPORT_API bool operator >=(const String& left, const String& right);
BGE_EXPORT_API String operator +(const String& left, const String& right);

void BGE_EXPORT_API splitAndConvert(const std::string& s,char flag,String& string);

template <typename T>
inline std::string toString(const T& v)
{
    std::ostringstream  os;
    os << v;
    return os.str();
}

inline String toBGEString(uint16_t value)
{
    String s(value);
    return s;
}

inline std::string toString(const bool& v)
{
    return v ? "true" : "false";
}

template <typename T>
inline T fromString(const std::string& str)
{
    T ret;
    std::istringstream is(str);
    is >> ret;
    return ret;
};

template <>
inline String fromString<String>(const std::string& str)
{
	String value;
	splitAndConvert(str,',',value);
	return value;
};

template <>
inline bool fromString<bool>(const std::string& str)
{
    return str == "true";
};

}

#endif

