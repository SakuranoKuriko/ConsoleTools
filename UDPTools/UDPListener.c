#include <stdlib.h>
#include <stdio.h>
#include <windows.h>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

#define SOCKET_BUFFER_SIZE 8192

void help(const char *name);

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
    int port = atoi(argv[1]);
    if (port<1){
        help(name);
        return 1;
    }

    char *target = "null";
    if (argc>2)
        target = argv[2];

    struct sockaddr_in servaddr;
    struct sockaddr_in caddr;
    int addrlen = sizeof(caddr);
    memset(&servaddr, 0, addrlen);
    memset(&caddr, 0, addrlen);

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData)!=0){
        printf("socket startup failed!");
        return 2;
    }
    SOCKET slisten = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (slisten == INVALID_SOCKET){
        printf("socket error!");
        return 2;
    }

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(slisten, (LPSOCKADDR) &servaddr, addrlen) == SOCKET_ERROR){
        printf("port bind failed!");
        return 2;
    }

    const int bufsize = sizeof(char)*SOCKET_BUFFER_SIZE;
    char *buf = malloc(bufsize);
    int datalen;
    FILE *f = NULL;
    if (strcmp(target, "null")!=0){
        f = fopen(target, "ab");;
        printf("Target file: %s\n", target);
    }
    printf("Listening on [0.0.0.0:%d]...\n", port);
    while (1){
        buf[0] = '\0';
        datalen = recvfrom(slisten, buf, bufsize, 0, (LPSOCKADDR) &caddr, &addrlen);
        printf("Receiving from [%s:%d]...\n", inet_ntoa(caddr.sin_addr), htons(caddr.sin_port));
        if (datalen<0){
            printf("receive error!\n");
            continue;
        }
        buf[datalen] = '\0';
        printf("%d %s\n", datalen, buf);
        if (strcmp(buf, "quit")==0)
            break;
        if (f!=NULL){
            if (fputs(buf, f)<0){
                printf("cannot write file!");
                return 3;
            }
            fputc('\n', f);
            fflush(f);
        }
    }

    printf("Cleaning up...");
    free(buf);
    if (f!=NULL)
        fclose(f);
    closesocket(slisten);
    WSACleanup();

    return 0;
}

void help(const char *name){
    printf("Usage: %s <port> [target filename(default:'null')]", name);
}
