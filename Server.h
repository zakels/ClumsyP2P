#pragma once

#include "define.h"
#include <WinInet.h>
#include <Ws2tcpip.h>

typedef struct _udpInfo{
	WSADATA wsaData;			//���� ������ ����ü
	SOCKET socket;				//����
	SOCKADDR_IN info;			//���� ����
	SOCKADDR_IN recvInfo;		//Ŭ���̾�Ʈ���� �޴� �ּ� ����
}UDPINFO, *LPUDPINFO;

void CreateServer(LPCSTR ip);