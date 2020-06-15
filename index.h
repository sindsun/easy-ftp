/****************************
* title: ftp主程序类
* description: 学习ftp协议与c++通信篇的一个学习项目，大致流程已经以及程序已经探索完毕，这个项目共享给学习需要的朋友，希望能帮助到一部分人
* datetime: 2020年6月15日16:11:19
* author: Sindsun
* from: http://www.sindsun.com
****************************/
#ifndef INDEX_H
#define INDEX_H

#include <iostream>
#include <string>
#include <winsock2.h>
#include <fstream>
#include <windows.h>
#include <iomanip>
#include <locale>

#include "common.h"

using namespace std;

#define MAX_MSG_SIZE 1024


class Index{
public:
    Index();
    ~Index();
    //程序开始
    void appStart();
private:
    Common *pCommon;

    enum TransMode{
        AUTO, PORT, PASV
    };

    //主机名
    string hostName = "";
    //端口号
    int portNum = 0;
    //用户名
    string userName = "";
    //密码
    string userPwd = "";


    //操作命令socket
    SOCKET ctrlSock;
    //数据连接socket
    SOCKET dataConnSock;
    //数据传输socket
    SOCKET dataTransSock;


    //命令字符串
    string requestString = "";
    //接收的响应字符串
    string responseString = "";
    //响应码
    unsigned int replyCode = 0;
    //主动模式端口号计数
    int portDataConnCount = 0;
    //错误消息
    string errorMsg = "";
    //选择的连接模式
    TransMode transModeVal = TransMode::AUTO;

    //网络组件初始化
    bool initSocketLib();
    //登录
    bool login();
    //退出登录
    bool logout();
    //菜单列表
    void menuList();
    //本地命令输入处理
    void localInput();
    //远程命令发送
    bool sendToRemote(bool returnReplyCode=false);
    //接收远程响应
    bool recvFromRemote();
    //处理远程响应，返回响应码
    bool getReplyCode();
    //写日志文件
    bool writeLogs(string msg);
    //显示错误
    void error(string errorStr);
    //显示成功
    void success(string sucStr);
    //按值选择数据传输模式
    bool transModelSelect();
    //PORT主动传输模式
    bool portMode();
    //允许远程连接本机
    bool allowAccept();
    //PASV连接模式
    bool pasvMode();
    //ftp参数初始化设置
    bool initFtpParam();

    //获取文件列表
    bool getList(string dirPath);
    //编辑名称
    bool setRename(string fileName);
    //上传文件
    bool uploadFile(string filePath);
    //下载文件
    bool downloadFile(string fileName);
    //删除文件
    bool deleteFile(string fileName);

};

#endif // INDEX_H
