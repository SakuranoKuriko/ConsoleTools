#include <stdlib.h>
#include <stdio.h>
#include <windows.h>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

#define CHECK_OK 0
#define CHECK_FAILED -1

//显示帮助
void help(const char *name);
//检查IP是否合法，返回CHECK_OK/CHECK_FAILED
int checkIP(char *str);
//调用atoi转换为int并检查是否在1~65535范围内，转换失败返回CHECK_FAILED
int toPort(char *str);
//从左往右搜索第一个匹配字符，返回其索引，未找到返回-1
int indexOf(char *str, char target);

int main(int argc, char *argv[]){
    //取得自身文件名
    char *name = argv[0];
    name = name + strlen(argv[0]);
    while(name!=argv[0] && *name--!='\\')
    if (*name=='\\')
        name++;
    else {name = argv[0];}
    if (argc<2){
        help(name);
        return 1;
    }

    //分割IP:PORT
    int len = strlen(argv[1]);
    int i = indexOf(argv[1], ':');
    if (i==-1 || i+1==len){
        printf("Illegal arguments!");
        return 1;
    }

    //检查IP
    char *ip = malloc(sizeof(char)*(i+1));
    *(ip+i) = '\0';
    memcpy(ip, argv[1], i);
    if (checkIP(ip)==CHECK_FAILED){
        printf("Illegal IP Address!");
        return 1;
    }

    //检查PORT
    int port = toPort(argv[1]+i+1);
    if (port==-1){
        printf("Illegal Port!");
        return 1;
    }

    //省略消息内容则发送'quit'关闭监听器
    char *msg;
    if (argc>2)
        msg = argv[2];
    else msg = "quit";

    //提示一下将要执行的操作信息
    printf("target: %s:%d message: %s\n", ip, port, msg);

    //接收者对象
    struct sockaddr_in addr;
    int addrlen = sizeof(addr);
    //内存空间置0
    memset(&addr, 0, addrlen);

    //AF_INET：IPv4
    //AF_INET6：IPv6
    //AF_UNIX：本机通信
    addr.sin_family = AF_INET;
    //目标端口
    addr.sin_port = htons(port);
    //目标地址
    addr.sin_addr.s_addr = inet_addr(ip);
    free(ip);

    //使用主,副版本号初始化WSA（Windows Sockets Asynchronous）
    //使用WSADATA接收初始化完成后的相关环境数据
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData)!=0){
        printf("socket startup failed!");
        return 2;
    }
    //创建套接字
    //SOCK_DGRAM：UDP数据报
    //SOCK_STREAM：TCP流
    //SOCK_RAW：原始套接字
    //最后一个参数可以设为0来自动使用默认值
    SOCKET sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sockfd == INVALID_SOCKET){
        printf("socket error!");
        return 2;
    }

    //发送UDP报文，sendto(套接字, 内容, 内容长度, flag, 目标, 目标对象长度)
    int s = sendto(sockfd, msg, strlen(msg)*sizeof(*msg), 0, (LPSOCKADDR)&addr, addrlen);
    if (s==SOCKET_ERROR)
        printf("Send failed!");
    else printf("Send success");

    //printf("Cleaning up...");
    closesocket(sockfd);
    //与WSAStartup配套的WSACleanup
    WSACleanup();

    return 0;
}

void help(const char *name){
    printf("Usage: %s <targetIP:targetPort> [message(default:'quit')]", name);
}

int checkIP(char *str){
    int mflag = 0; //'.'出现次数
    int nflag = 0; //数字出现次数
    int count = 0; //总长度
    while(*str!='\0'){
        if (*str=='.')
            mflag++;
        else if (*str>='0' && *str<='9')
            nflag++;
        else return CHECK_FAILED;
        if (mflag==4 || nflag==19)
            return CHECK_FAILED;
        str++;
        if (++count==23)
            return CHECK_FAILED;
    }
    return CHECK_OK;
}

int toPort(char *str){
    int port = atoi(str);
    if (port>0 && port<65536)
        return port;
    return CHECK_FAILED;
}

int indexOf(char *str, char target){
    int i = 0;
    char *iter = str;
    while (*iter!='\0' && *iter!=target){
        iter++;
        i++;
    }
    if (*iter!=target)
        i = -1;
    return i;
}
