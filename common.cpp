#include "common.h"

Common::Common()
{

}

//将utf8转换为GBK,windows引入windows.h
string Common::utf8ToGbk(const char *src_str)
{
    int len = MultiByteToWideChar(CP_UTF8, 0, src_str, -1, nullptr, 0);
    wchar_t* wszGBK = new wchar_t[len + 1];
    memset(wszGBK, 0, static_cast<unsigned int>(len * 2 + 2));
    MultiByteToWideChar(CP_UTF8, 0, src_str, -1, wszGBK, len);
    len = WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, nullptr, 0, nullptr, nullptr);
    char* szGBK = new char[len + 1];
    memset(szGBK, 0, static_cast<unsigned int>(len + 1));
    WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, szGBK, len, nullptr, nullptr);
    string strTemp(szGBK);
    if (wszGBK) delete[] wszGBK;
    if (szGBK) delete[] szGBK;
    return strTemp;
}

//将gbk编码字符转换为utf8
string Common::gbkToUtf8(const char *src_str)
{
    int len = MultiByteToWideChar(CP_ACP, 0, src_str, -1, nullptr, 0);
    wchar_t* wstr = new wchar_t[len + 1];
    memset(wstr, 0, static_cast<unsigned int>(len + 1));
    MultiByteToWideChar(CP_ACP, 0, src_str, -1, wstr, len);
    len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, nullptr, 0, nullptr, nullptr);
    char* str = new char[len + 1];
    memset(str, 0, static_cast<unsigned int>(len + 1));
    WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, len, nullptr, nullptr);
    string strTemp = str;
    if (wstr) delete[] wstr;
    if (str) delete[] str;
    return strTemp;
}
