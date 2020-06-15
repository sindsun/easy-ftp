/****************************
* title: 辅助处理的公共函数类
* description: 学习ftp协议与c++通信篇的一个学习项目，大致流程已经以及程序已经探索完毕，这个项目共享给学习需要的朋友，希望能帮助到一部分人
* datetime: 2020年6月15日16:11:19
* author: Sindsun
* from: http://www.sindsun.com
****************************/
#ifndef COMMON_H
#define COMMON_H

#include <iostream>
#include <string>
#include <windows.h>
#include <locale.h>
#include <codecvt>
#include <locale>

using namespace std;

class Common{
public:
    Common();
    ~Common();

    //将utf8转换为GBK,windows引入windows.h
    string utf8ToGbk(const char *src_str);
    //将gbk编码字符转换为utf8
    string gbkToUtf8(const char *src_str);
};

#endif // COMMON_H
