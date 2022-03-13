#pragma once
#include<vector>
#include<string>
#include<iostream>
#include <codecvt>
#include <locale>

using convert_t = std::codecvt_utf8<wchar_t>;
static std::wstring_convert<convert_t, wchar_t> strconverter;

namespace Duality
{

template<class STRING_TYPE>
STRING_TYPE toUpper(const STRING_TYPE& str)
{
    STRING_TYPE ret=str;
    for(auto& ch:ret)
    {
        if(ch>='a' && ch<='z')
        {
            ch-='a'-'A';
        }
    }
    return ret;
}

template<class STRING_TYPE>
STRING_TYPE toLower(const STRING_TYPE& str)
{
    STRING_TYPE ret=str;
    for(auto& ch:ret)
    {
        if(ch>='A' && ch<='Z')
        {
            ch+='a'-'A';
        }
    }
    return ret;
}


template<class STRING_TYPE>
struct Dstring
{
    friend std::ostream& operator<<(std::ostream& ostrm, const Dstring& ds)
    {
        ostrm<<ds.data;
        return ostrm;
    }
    STRING_TYPE data;
    Dstring(const STRING_TYPE& str):data{str}{}
    Dstring toUpper()
    {
        Dstring ret(data);
        for(auto& ch:ret.data)
        {
            if(ch>='a' && ch<='z')
            {
                ch-='a'-'A';
            }
        }
        return ret;
    }
    Dstring toLower()
    {
        Dstring ret(data);
        for(auto& ch:ret.data)
        {
            if(ch>='A' && ch<='Z')
            {
                ch+='a'-'A';
            }
        }
        return ret;
    }
    decltype(STRING_TYPE::npos) find(const Dstring& target)
    {
        return data.find(target.data);
    }
};

} // namespace Duality
