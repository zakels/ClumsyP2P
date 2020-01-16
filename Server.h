#pragma once

#include "define.h"
#include <WinInet.h>
#include <Ws2tcpip.h>

typedef struct _udpInfo{
	WSADATA wsaData;			//원속 데이터 구조체
	SOCKET socket;				//소켓
	SOCKADDR_IN info;			//서버 정보
	SOCKADDR_IN recvInfo;		//클라이언트에서 받는 주소 정보
}UDPINFO, *LPUDPINFO;

void CreateServer(LPCSTR ip);