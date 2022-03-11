#pragma once
#include<vector>
#include<string>
#include<iostream>
namespace Duality
{
struct Dstring
{
    friend std::ostream& operator<<(std::ostream& ostrm, const Dstring& ds)
    {
        ostrm<<ds.data;
        return ostrm;
    }
    std::string data;
    Dstring(const std::string& str):data{str}{}
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
    decltype(std::string::npos) find(const Dstring& target)
    {
        return data.find(target.data);
    }
};

} // namespace Duality
