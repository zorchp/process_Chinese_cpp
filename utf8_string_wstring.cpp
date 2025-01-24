#include <string>
#include <iostream>
#include <boost/locale.hpp> // for utf8 wstring <=> utf8 string

#define _logid 0
#define WLOG printf

std::string wstring_to_utf8(const std::wstring& wstr);
std::wstring utf8_to_wstring(const std::string& str);

std::string wstring_to_utf8(const std::wstring& wstr) {
    try {
        return boost::locale::conv::from_utf(wstr, "UTF-8");
    } catch (const boost::locale::conv::conversion_error& e) {
        WLOG(
            "[wstring_to_utf8][logid=%d] convert wstring to UTF-8 failed: %s\n",
            _logid, e.what());
        return "";
    } catch (const std::exception& e) {
        WLOG("[wstring_to_utf8][logid=%d] unknown error: %s\n", _logid,
             e.what());
        return "";
    }
}

std::wstring utf8_to_wstring(const std::string& str) {
    try {
        return boost::locale::conv::to_utf<wchar_t>(str, "UTF-8");
    } catch (const boost::locale::conv::conversion_error& e) {
        WLOG(
            "[utf8_to_wstring][logid=%d] convert UTF-8 to wstring failed: %s\n",
            _logid, e.what());
        return L"";
    } catch (const std::exception& e) {
        WLOG("[utf8_to_wstring][logid=%d] unknown error: %s\n", _logid,
             e.what());
        return L"";
    }
}

// clang++ -std=c++17  utf8_string_wstring.cpp -lboost_locale
int main() {
    std::string str = "hello, 你好，世界！";
    std::wstring wstr = utf8_to_wstring(str);
    std::cout<<wstr.size()<<std::endl;
    std::string str2 = wstring_to_utf8(wstr);
    std::cout << str2 << std::endl;
    return 0;
}