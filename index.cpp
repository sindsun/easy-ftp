#include "index.h"

//构造函数
Index::Index()
{
    this->pCommon = new Common;

    if(!this->initSocketLib()){
        this->error(this->errorMsg);
        exit(-1);
    }
}
//析构函数
Index::~Index()
{
    closesocket(this->ctrlSock);
    closesocket(this->dataConnSock);
    closesocket(this->dataTransSock);
    WSACleanup();
}
//程序开始
void Index::appStart()
{
    cout << endl << "欢迎使用sindftp! (本软件出自www.sindsun.com)" <<endl << endl;

    if(!this->login()){
        this->error(this->errorMsg);
        cout << "error code is " << replyCode << endl;
        Sleep(3000);
        system("cls");
        this->appStart();
    }
    this->menuList();
}
//登录
bool Index::login()
{
    cout << endl << setw(12) << "主机地址: "  ;
    cin >> this->hostName;

    cout << endl << setw(12) << "端口号: ";
    cin >> this->portNum;

    cout << endl << setw(12) << "账号: ";
    cin >> this->userName;

    cout << endl << setw(12) << "密码: ";
    cin >> this->userPwd;
    cout << endl << endl;

    this->ctrlSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(this->ctrlSock == static_cast<unsigned>(SOCKET_ERROR)){
        this->error("创建socket失败");
        exit(-1);
    }

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(static_cast<unsigned short>(this->portNum));
    addr.sin_addr.s_addr = inet_addr(this->hostName.c_str());
    int connStatus = connect(this->ctrlSock, reinterpret_cast<sockaddr *>(&addr), sizeof (addr));
    if(connStatus == -1){
        this->errorMsg = "连接服务器失败";
        return false;
    }
    if(!this->recvFromRemote() || this->replyCode != 220){
        this->errorMsg = "连接服务器错误";
        return false;
    }

    //进入登录
    this->requestString = "USER " + this->userName;
    if(!sendToRemote(true) || this->replyCode != 331){
        this->errorMsg = "用户名错误";
        return false;
    }
    this->requestString = "PASS " + this->userPwd;
    if(!sendToRemote(true) || this->replyCode != 230){
        this->errorMsg = "用户名或密码错误";
        return false;
    }

    return true;
}
//菜单列表
void Index::menuList()
{
    cout << "----------- sindftp ------------" << endl << endl;
    cout << "--------------------------------" << endl << endl;
    cout << "显示菜单\t menu" << endl << endl;
    cout << "切换目录\t cwd dirname" << endl << endl;
    cout << "文件列表\t list dirname" << endl << endl;
    //cout << "文件重命名\t rename filename" << endl << endl;
    cout << "上传文件\t upload filename" << endl << endl;
    cout << "下载文件\t download filename" << endl << endl;
    cout << "删除文件\t del filename" << endl << endl;
    cout << "帮助信息\t help" << endl << endl;
    cout << "清除屏幕\t clear" << endl << endl;
    cout << "退出平台\t quit" << endl << endl;
    cout << "--------------------------------" << endl;
    cout << endl;

    localInput();
}
//本地命令输入处理
void Index::localInput()
{
    requestString = "";
    responseString = "";
    errorMsg = "";

    string userInputCmd = "";
    cout << endl;
    cout << "input cmd ### " ;
    cin.sync();
    getline(cin, userInputCmd);

    if(userInputCmd.length() < 1){
        errorMsg = "请输入正确的命令！";
        localInput();
        return;
    }
    //分割命令与参数
    string cmd = "";
    string param = "";
    bool cmdOver = false;
    for(char ch:userInputCmd){
        if(ch == ' '){
            cmdOver = true;
            continue;
        }

        if(cmdOver){
            param += ch;
        }else{
            cmd += ch;
        }
    }
    //处理命令
    if(cmd == "help"){  //帮助
        requestString = "HELP";
        sendToRemote(true);
    }else if(cmd == "menu"){  //菜单
        menuList();
    }else if(cmd == "cwd"){  //改变目录
        //改变数据目录
        requestString = "CWD "+param;
        sendToRemote(true);
    }else if(cmd == "list"){  //列表
        getList(param);
    }else if(cmd == "rename"){  // 重命名

    }else if(cmd == "upload"){  //上传
        uploadFile(param);
    }else if(cmd == "download"){  //下载
        downloadFile(param);
    }else if(cmd == "del"){  //删除
        deleteFile(param);
    }else if(cmd == "clear"){  //清除屏幕
        system("cls");
        menuList();
    }else if(cmd == "quit"){  //退出
        cout << "Bye bye!" <<endl;
        exit(-1);
    }else{
        //errorMsg = "请输入正确的命令！";
        //error(errorMsg);
        requestString = userInputCmd;
        sendToRemote(true);
    }

    localInput();
}
//远程命令发送
bool Index::sendToRemote(bool returnReplyCode)
{
    if(this->requestString.length() < 1){
        this->errorMsg = "ftp命令不能为空";
        return false;
    }
    cout << endl << setw(10) << "发送命令: " << setw(10) << requestString << endl;

    requestString += "\r\n";
    requestString = this->pCommon->gbkToUtf8(requestString.c_str());
    unsigned int cmdLength = requestString.length();
    char *tmpCmdStr = new char[cmdLength];
    memset(tmpCmdStr, 0, cmdLength);
    memcpy(tmpCmdStr, requestString.c_str(), cmdLength);

    int sendStatus = send(this->ctrlSock, tmpCmdStr, static_cast<int>(cmdLength), 0);
    delete [] tmpCmdStr;
    if(sendStatus == -1){
        this->errorMsg = "请求失败";
        return false;
    }
    if(returnReplyCode){
        return recvFromRemote();
    }
    return true;
}
//接收远程响应
bool Index::recvFromRemote()
{
    if(this->ctrlSock == INVALID_SOCKET){
        this->errorMsg = "服务已断开 ";
        return false;
    }
    char buf[MAX_MSG_SIZE];
    int recvStatus = -1;
    responseString = "";

    while(true){
        memset(buf, 0, MAX_MSG_SIZE);
        Sleep(500);
        recvStatus = recv(this->ctrlSock, buf, MAX_MSG_SIZE, MSG_PARTIAL);
        //cout << "######### " << buf << endl;
        if(recvStatus > 0){
            responseString = buf;
            break;
        }
    }
    if(responseString.length() == 0){
        this->errorMsg = "接收数据失败";
        return false;
    }
    this->success(responseString);

    return this->getReplyCode();
}
//处理远程响应，返回响应码
bool Index::getReplyCode()
{
    string tmpMsg = "";
    tmpMsg = responseString.substr(0, 3);
    char charCode[3] = {};

    memcpy(charCode, tmpMsg.c_str(), tmpMsg.length());
    this->replyCode = static_cast<unsigned int>(atoi(charCode));
    if( !(this->replyCode >= 1 && this->replyCode <= 1000)){
        this->errorMsg = "无法获取到响应码";
        return false;
    }

    return true;
}
//写日志文件
bool Index::writeLogs(string msg)
{
    if(msg.length() == 0){
        return false;
    }
    SYSTEMTIME sTime;
    GetLocalTime(&sTime);
    char fullTime[20];
    memset(fullTime, 0, 20);
    sprintf(fullTime, "%4d-%02d-%02d %02d:%02d:%02d", sTime.wYear, sTime.wMonth, sTime.wDay,
            sTime.wHour, sTime.wMinute, sTime.wSecond);
    msg = "\r\n" + msg;
    msg = fullTime + msg;

    fstream fs;
    fs.open("d:/test/datasock_recv_logs.log", ios::app);

    fs << msg << endl;

    fs.close();

    return true;
}
//显示错误
void Index::error(string errorStr)
{
    if(errorStr.length() > 0){
        cout << endl;
        cout << "Error: " << errorStr;
        cout << endl;
    }
}
//显示成功
void Index::success(string sucStr)
{
    if(sucStr.length() > 0){
        cout << endl;
        cout << "服务响应:" << endl;
        cout << sucStr;
        cout << endl;
    }
}
//按值选择数据传输模式
bool Index::transModelSelect()
{
    if(transModeVal == TransMode::PORT){
        return portMode();
    }else if(transModeVal == TransMode::PASV){
        return pasvMode();
    }else{
        return (pasvMode() || portMode());
    }
}
//PORT主动传输模式
bool Index::portMode()
{
    dataConnSock = socket(AF_INET, SOCK_STREAM, 0);
    if(dataConnSock == static_cast<unsigned>(SOCKET_ERROR)){
        errorMsg = "初始化数据socket失败";
        return false;
    }

    portDataConnCount += 1;

    sockaddr_in addr;
    int addrLength = sizeof (addr);
    int getsocknameStatus = getsockname(ctrlSock, reinterpret_cast<sockaddr *>(&addr), &addrLength);
    if(getsocknameStatus == -1){
        errorMsg = "获取socket信息失败";
        return false;
    }
    int localPort = ntohs(addr.sin_port) + portDataConnCount;   //获取机地随机端口+1

    addr.sin_family = AF_INET;
    addr.sin_port = htons(static_cast<u_short>(localPort));
    //addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int countBind = 0;
    int bindStatus = -1;
    while(bindStatus != 0){
        bindStatus = bind(dataConnSock, reinterpret_cast<sockaddr *>(&addr), sizeof (addr));
        countBind ++;
        if(countBind >= 10){
            break;
        }
    }
    if(bindStatus == -1){
        errorMsg = "绑定失败";
        return false;
    }

    //主动模式下的监听一下要放到发送了请求数据命令之后
    int listenStatus = listen(dataConnSock, 64);
    if(listenStatus == -1){
        errorMsg = "监听失败";
        return false;
    }

    char *ipAddr = new char[32];
    memset(ipAddr, 0, 32);
    ipAddr = inet_ntoa(addr.sin_addr);

    //计算端口号
    char charPortNum1[5];
    char *portNum1 = itoa(localPort/256, charPortNum1, 10);
    char charPortNum2[5];
    char *portNum2 = itoa(localPort%256, charPortNum2, 10);

    string portModelStr = ipAddr;
    portModelStr += ",";
    portModelStr += portNum1;
    portModelStr += ",";
    portModelStr += portNum2;
    //替换里面的特殊字符
    for(char &c:portModelStr){
        if(c == '.'){
            c = ',';
        }
    }
    requestString = "PORT " + portModelStr;
    if(!sendToRemote(true)){
        errorMsg = "发送port命令失败";
        return false;
    }

    transModeVal = TransMode::PORT;

    success("PORT模式开启成功");

    return true;
}
//允许远程连接本机
bool Index::allowAccept()
{
    if(transModeVal != TransMode::PORT){
        return true;
    }
    sockaddr_in acceptAddr;
    int acceptAddrLength = sizeof (acceptAddr);
    dataTransSock = accept(dataConnSock, reinterpret_cast<sockaddr *>(&acceptAddr), &acceptAddrLength);
    if(dataTransSock == static_cast<u_short>(INVALID_SOCKET)){
        errorMsg = "接受请求失败";
        return false;
    }
    return true;
}
//PASV连接模式
bool Index::pasvMode()
{
    dataTransSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(dataTransSock == static_cast<u_short>(SOCKET_ERROR)){
        errorMsg = "PASV socket初始化失败";
        return false;
    }
    requestString = "PASV";
    if(!sendToRemote(true) || replyCode != 227){
        errorMsg = "PASV设置失败";
        return false;
    }
    //处理字符串
    char *newChar = strrchr(responseString.c_str(), '(') + 1;
    string tmpStr = newChar + 1;
    int spCount = 0;
    string p1 = "";
    string p2 = "";
    for(char ch:tmpStr){
        if(ch == ')'){
            break;
        }
        if(ch == ','){
            spCount ++;
            continue;
        }
        if(spCount > 3){
            if(spCount == 4){
                p1 += ch;
            }else{
                p2 += ch;
            }
        }
    }
    int dataPort = atoi(p1.c_str()) * 256 + atoi(p2.c_str());
    cout << "p1 " << p1 << endl;
    cout << "p2 " << p2 << endl;
    cout << "port " << dataPort << endl;
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(static_cast<u_short>(dataPort));
    addr.sin_addr.s_addr = inet_addr(hostName.c_str());
    int ret = connect(dataTransSock, reinterpret_cast<sockaddr *>(&addr), sizeof (addr));
    if(ret == -1){
        errorMsg = "PASV连接服务器失败";
        error(errorMsg);
        closesocket(dataTransSock);
        return false;
    }

    transModeVal = TransMode::PASV;

    success("PASV模式开启成功");

    return true;
}
//获取文件列表
bool Index::getList(string dirPath)
{
    if(!transModelSelect()){
        error(errorMsg);
        return false;
    }
    requestString = "LIST "+dirPath;
    if(!sendToRemote(true) || replyCode != 150 ){
        error(errorMsg);
        return false;
    }

    if(!allowAccept()){
        error(errorMsg);
        return false;
    }

    char *recvData = new char[MAX_MSG_SIZE];
    memset(recvData, 0 , MAX_MSG_SIZE);
    //如果是150,需要再接收一次数据发送结束的通知
    int recvStatus = recv(dataTransSock, recvData, MAX_MSG_SIZE, 0);
    if(recvStatus == -1){
        error("接收数据失败");
        return false;
    }

    success(pCommon->utf8ToGbk(recvData));
    writeLogs(recvData);
    success("写入日志文件成功");
    delete [] recvData;


    return true;
}
//上传文件
bool Index::uploadFile(string filePath)
{
    if(!transModelSelect()){
        return false;
    }
    char *fileName = strrchr(filePath.c_str(), '/');
    requestString = "STOR ";
    requestString += fileName +1;
    if(!sendToRemote(true)){
        errorMsg = "命令发送失败";
        return false;
    }
    if(!allowAccept()){
        return false;
    }

    fstream fs;
    fs.open(filePath, ios::in);
    char buf[MAX_MSG_SIZE];
    memset(buf, 0, MAX_MSG_SIZE);
    bool status = true;
    while(!fs.eof()){
        fs.read(buf, MAX_MSG_SIZE);
        int storStatus = send(dataTransSock, buf, sizeof (buf), 0);
        if(storStatus == -1){
            errorMsg = "上传文件异常";
            status = false;
            break;
        }
    }
    closesocket(dataTransSock);
    if(!status){
        return false;
    }

    return true;
}
//下载文件
bool Index::downloadFile(string fileName)
{
    //先获取文件大小
    requestString = "SIZE ";
    requestString += fileName;
    if(!sendToRemote(true)){
        errorMsg = "命令发送失败";
        return false;
    }
    int fileSize = atoi(responseString.substr(4).c_str());

    if(!transModelSelect()){
        return false;
    }
    requestString = "RETR " + fileName;
    if(!sendToRemote(true)){
        errorMsg = "命令发送失败";
        return false;
    }
    if(!allowAccept()){
        errorMsg = "连接失败";
        return false;
    }
    //开始进行下载动作
    string localPath = "d:/test/d_" + fileName;
    fstream fs;
    fs.open(localPath, ios::app);
    int countFileSize = 0;
    char *buf = new char[MAX_MSG_SIZE];
    while (countFileSize < fileSize) {
        memset(buf, 0, MAX_MSG_SIZE);
        recv(dataTransSock, buf, MAX_MSG_SIZE, 0);
        if(strlen(buf) < 1){
            break;
        }
        fs << buf ;
        countFileSize += strlen(buf);
    }
    fs.close();

    return true;
}
//删除文件
bool Index::deleteFile(string fileName)
{
    if(!transModelSelect()){
        return false;
    }
    requestString = "DELE " + fileName;
    if(!sendToRemote(true)){
        errorMsg = "命令发送失败";
        return false;
    }


    return true;
}
//网络组件初始化
bool Index::initSocketLib()
{
    WSADATA wsa;
    WORD vCode = MAKEWORD(2, 2);
    int wsaStartupStatus = WSAStartup(vCode, &wsa);
    if(wsaStartupStatus == -1){
        this->errorMsg = "初始化套接字库失败，请重试";
        return false;
    }
   //检查套接字版本号
   if(LOBYTE(wsa.wVersion) != 2 || HIBYTE(wsa.wHighVersion) != 2){
       WSACleanup();
       this->errorMsg = "套接字版本号不符合，请重新配置";
       return false;
   }
   return true;
}
