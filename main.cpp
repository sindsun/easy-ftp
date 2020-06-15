/****************************
* title: ftp学习程序 学习成果共享
* description: 学习ftp协议与c++通信篇的一个学习项目，大致流程已经以及程序已经探索完毕，这个项目共享给学习需要的朋友，希望能帮助到一部分人
* datetime: 2020年6月15日16:11:19
* author: Sindsun
* from: http://www.sindsun.com
****************************/
#include <iostream>
#include "index.h"

using namespace std;

int main()
{
    system("mode con: cols=120 lines=500");
    system("color 1f");

    Index *index = new Index;
    index->appStart();

    return 0;
}
