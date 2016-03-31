#pragma once
#include <vector>
#include <string>
#include <codecvt>

using ulong = unsigned long;
using uint = unsigned int;
using byte = unsigned char;

template<typename T>
bool contains(std::vector<T> vector, T value)
{
    auto it = std::find(vector.begin(), vector.end(), value);

    return it != vector.end();
}

inline std::wstring s2ws(const std::string& str)
{
    typedef std::codecvt_utf8<wchar_t> convert_typeX;
    std::wstring_convert<convert_typeX, wchar_t> converterX;

    return converterX.from_bytes(str);
}

inline std::string ws2s(const std::wstring& wstr)
{
    typedef std::codecvt_utf8<wchar_t> convert_typeX;
    std::wstring_convert<convert_typeX, wchar_t> converterX;

    return converterX.to_bytes(wstr);
}