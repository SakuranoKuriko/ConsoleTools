#include <stdlib.h>
#include <stdio.h>
#include <windows.h>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

#define CHECK_OK 0
#define CHECK_FAILED -1

void help(const char *name);
int checkIP(char *str);
int toPort(char *str);
int indexOf(char *str, char target);

int main(int argc, char *argv[]){
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

    int len = strlen(argv[1]);
    int i = indexOf(argv[1], ':');
    if (i==-1 || i+1==len){
        printf("Illegal arguments!");
        return 1;
    }

    char *ip = malloc(sizeof(char)*(i+1));
    *(ip+i) = '\0';
    memcpy(ip, argv[1], i);
    if (checkIP(ip)==CHECK_FAILED){
        printf("Illegal IP Address!");
        return 1;
    }

    int port = toPort(argv[1]+i+1);
    if (port==-1){
        printf("Illegal Port!");
        return 1;
    }

    char *msg;
    if (argc>2)
        msg = argv[2];
    else msg = "quit";

    printf("target: %s:%d message: %s\n", ip, port, msg);

    struct sockaddr_in addr;
    int addrlen = sizeof(addr);
    memset(&addr, 0, addrlen);

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip);
    free(ip);

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData)!=0){
        printf("socket startup failed!");
        return 2;
    }
    SOCKET sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sockfd == INVALID_SOCKET){
        printf("socket error!");
        return 2;
    }

    int s = sendto(sockfd, msg, strlen(msg)*sizeof(*msg), 0, (LPSOCKADDR)&addr, addrlen);
    if (s==SOCKET_ERROR)
        printf("Send failed!");
    else printf("Send success");

    printf("Cleaning up...");
    closesocket(sockfd);
    WSACleanup();

    return 0;
}

void help(const char *name){
    printf("Usage: %s <targetIP:targetPort> [message(default:'quit')]", name);
}

int checkIP(char *str){
    int mflag = 0;
    int nflag = 0;
    int count = 0;
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
