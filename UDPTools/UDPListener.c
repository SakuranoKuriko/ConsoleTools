#include <stdlib.h>
#include <stdio.h>
#include <windows.h>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

//接收数据时的缓冲区大小
#define SOCKET_BUFFER_SIZE 8192

//显示帮助
void help(const char *name);

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
    
    //监听端口号
    int port = atoi(argv[1]);
    if (port<1){
        help(name);
        return 1;
    }

    //默认为null即不输出至文件
    char *target = "null";
    if (argc>2)
        target = argv[2];

    //服务器对象，就是监听者
    struct sockaddr_in servaddr;
    //客户端对象，就是发送者
    struct sockaddr_in caddr;
    int addrlen = sizeof(caddr);
    memset(&servaddr, 0, addrlen);
    memset(&caddr, 0, addrlen);
    
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
    SOCKET slisten = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (slisten == INVALID_SOCKET){
        printf("socket error!");
        return 2;
    }

    //AF_INET：IPv4
    //AF_INET6：IPv6
    //AF_UNIX：本机通信
    servaddr.sin_family = AF_INET;
    //监听端口
    servaddr.sin_port = htons(port);
    //监听地址，INADDR_ANY表示所有，即0:0:0:0
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    //套接字绑定监听者对象
    if (bind(slisten, (LPSOCKADDR) &servaddr, addrlen) == SOCKET_ERROR){
        printf("port bind failed!");
        return 2;
    }

    //创建缓冲区
    const int bufsize = sizeof(char)*SOCKET_BUFFER_SIZE;
    char *buf = malloc(bufsize);
    //接收到的数据长度
    int datalen;
    //写入文件目标
    FILE *f = NULL;
    if (strcmp(target, "null")!=0){
        f = fopen(target, "ab");;
        printf("Target file: %s\n", target);
    }
    printf("Listening on [0.0.0.0:%d]...\n", port);
    while (1){ //无限循环等待接收数据包
        buf[0] = '\0';
        //接收UDP报文，recvfrom(套接字, 写入目标缓冲区, 缓冲区大小, flag, 返回的发送者, 发送者对象长度)
        datalen = recvfrom(slisten, buf, bufsize, 0, (LPSOCKADDR) &caddr, &addrlen);
        printf("Receiving from [%s:%d]...\n", inet_ntoa(caddr.sin_addr), htons(caddr.sin_port));
        if (datalen<0){
            printf("receive error!\n");
            continue;
        }
        //截断缓冲区
        buf[datalen] = '\0';
        printf("%d %s\n", datalen, buf);
        if (strcmp(buf, "quit")==0)
            break;
        if (f!=NULL){
            //写入文件
            if (fputs(buf, f)<0){
                printf("cannot write file!");
                return 3;
            }
            //换个行
            fputc('\n', f);
            //清空stdout缓冲区来使它立即写入到文件
            fflush(f);
        }
    }

    printf("Cleaning up...");
    free(buf);
    if (f!=NULL)
        fclose(f);
    closesocket(slisten);
    //与WSAStartup配套的WSACleanup
    WSACleanup();

    return 0;
}

void help(const char *name){
    printf("Usage: %s <port> [target filename(default:'null')]", name);
}
