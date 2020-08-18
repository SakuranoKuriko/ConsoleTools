#include <stdlib.h>
#include <stdio.h>
#include <windows.h>

/* DCB结构
typedef struct _DCB {
    DWORD DCBlength;             DCB结构的长度
    DWORD BaudRate;              当前波特率
    DWORD fBinary: 1;            二进制模式，无EOF检查
    DWORD fParity: 1;            激活奇偶校验检查
    DWORD fOutxCtsFlow:1;        CTS输出流量控制
    DWORD fOutxDsrFlow:1;        DSR输出流量控制
    DWORD fDtrControl:2;         DTR流量控制类型
    DWORD fDsrSensitivity:1;     DSR安全设置
    DWORD fTXContinueOnXoff: 1;  XOFF持续Tx
    DWORD fOutX: 1;              XON/XOFF输出流量控制
    DWORD fInX: 1;               XON/XOFF输出流量控制
    DWORD fErrorChar: 1;         激活错误替换机制
    DWORD fNull: 1;              允许舍空格
    DWORD fRtsControl:2;         Rts流量控制
    DWORD fAbortOnError:1;       有错误时放弃读/写
    DWORD fDummy2:17;            保留
    WORD wReserved;              现在不使用
    WORD XonLim;                 传送XON阀值
    WORD XoffLim;                传送XOFF阀值
    BYTE ByteSize;               每个字节的位数 4-8
    BYTE Parity;                 校验 0-4=None,Odd,Even,Mark,Space
    BYTE StopBits;               0,1,2 = 1, 1.5, 2  建议使用常量ONESTOPBIT,ONE5STOPBITS,TWOSTOPBITS
    char XonChar;                设置Tx和Rx的XON的字符
    char XoffChar;               设置Tx和Rx的XOFF字符
    char ErrorChar;              有错误时的替换字符
    char EofChar;                表示输入结束的字符
    char EvtChar;                接收事件字符
    WORD wReserved1;             保留未使用
} DCB, *LPDCB;
*/

/* 串口超时时间结构，单位毫秒
typedef struct _COMMTIMEOUTS
{ DWORD ReadIntervalTimeout;            //读间隔超时
  DWORD ReadTotalTimeoutMultiplier;     //读总超时系数
  DWORD ReadTotalTimeoutConstant;       //读总超时常数
  DWORD WriteTotalTimeoutMultiplier;    //写总超时系数
  DWORD WriteTotalTimeoutConstant;      //写总超时常数
} COMMTIMEOUTS, *LPCOMMTIMEOUTS;        //读/写总超时时间=系数×字符数+常数
*/

DWORD getErr(){
    return GetLastError();
}
DCB dcb;
COMMTIMEOUTS timeouts;
HANDLE openComm(BYTE commNum, DWORD baudrate = 0, DWORD timeout = 0, BOOL async = FALSE){
    char commName[10] = {0};
    sprintf(commName, "\\\\.\\COM%d", commNum);
    HANDLE comm = CreateFile(commName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, (async==FALSE)?0:FILE_FLAG_OVERLAPPED, NULL);
    if (comm==INVALID_HANDLE_VALUE)
        return INVALID_HANDLE_VALUE;
    if (GetCommState(comm, &dcb)==FALSE){
        CloseHandle(comm);
        return INVALID_HANDLE_VALUE;
    }
    if (baudrate!=0){
        dcb.BaudRate = baudrate;
        if (SetCommState(comm, &dcb)==FALSE){
            CloseHandle(comm);
            return INVALID_HANDLE_VALUE;
        }
    }
    if (SetupComm(comm, 1024, 4096)==FALSE){
        CloseHandle(comm);
        return INVALID_HANDLE_VALUE;
    }
    if (GetCommTimeouts(comm, &timeouts)==FALSE){
        CloseHandle(comm);
        return INVALID_HANDLE_VALUE;
    }
    if (timeout!=0){
        timeouts.ReadTotalTimeoutConstant = timeout*1000;
        timeouts.WriteTotalTimeoutConstant = timeout*1000;
        if (SetCommTimeouts(comm, &timeouts)==FALSE){
            CloseHandle(comm);
            return INVALID_HANDLE_VALUE;
        }
    }
    return comm;
}
BOOL closeComm(HANDLE comm){
    return CloseHandle(comm);
}
BOOL send(HANDLE comm, const BYTE *data, DWORD datalen){
    DWORD writelen;
    if (WriteFile(comm, data, datalen, &writelen, NULL)==FALSE)
        return FALSE;
    PurgeComm(comm, PURGE_TXCLEAR);
    return TRUE;
}
DWORD recv(HANDLE comm, BYTE *data, DWORD readlen){
    DWORD readedlen;
    if (ReadFile(comm, data, readlen, &readedlen, NULL)==FALSE)
        return -1;
    return readedlen;
}

int main(int argc, char *argv[]){
    HANDLE comm = openComm(9);
    if (comm==INVALID_HANDLE_VALUE){
        printf("初始化串口失败(ErrorCode:%ld)", getErr());
        return 2;
    }
}