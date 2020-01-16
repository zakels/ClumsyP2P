#include "stdafx.h"
#include "Server.h"
#include "Client.h"
#include "Message.h"
#include "Node.h"
#include "sha1.h"
#include "FileStorageManager.h"
#include "RoutingManager.h"
#include <iostream>
#include <process.h>

extern RoutingManager g_RM;
extern FileStorageManager g_FSM;
extern Node g_myNode;


using namespace std;

// RoutingManager Only cares about controlling RoutingTable
// When requests are received from ServerHandler, it works out and pass to Client.

extern RoutingManager g_RM;

UINT CALLBACK UDPListener(LPVOID lpParam);

void CreateServer(LPCSTR ip)
{
	LPUDPINFO pUdpInfo = new UDPINFO;
	ZeroMemory(pUdpInfo, sizeof(UDPINFO));

	if (WSAStartup(0x202, &(pUdpInfo->wsaData)) == SOCKET_ERROR)
	{
		OutputDebugStringW(L"Error Initializing WinSock");
		WSACleanup();
		return;
	}
	
	memset(&(pUdpInfo->info), 0, sizeof(pUdpInfo->info));
	memset(&(pUdpInfo->recvInfo), 0, sizeof(pUdpInfo->recvInfo));

	pUdpInfo->info.sin_family = AF_INET;							// ipv4
	pUdpInfo->info.sin_addr.s_addr = INADDR_ANY;
	pUdpInfo->info.sin_port = htons(PORT_NO);

	pUdpInfo->socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);	// enable UDP Protocol
	if (pUdpInfo->socket == INVALID_SOCKET)
	{
		OutputDebugStringW(L"Error Creating Socket");
		closesocket(pUdpInfo->socket);
		WSACleanup();

		return;
	}

	if( bind( pUdpInfo->socket, (struct sockaddr*)&pUdpInfo->info, //바인드 소켓에 서버정보 부여
		sizeof(pUdpInfo->info) ) == SOCKET_ERROR )
	{
		cout<<"Unable to bind"<<endl;
		closesocket( pUdpInfo->socket );
		WSACleanup();
		return;
	}

	HANDLE hThread = (HANDLE) _beginthreadex(NULL, 0, UDPListener, pUdpInfo/*lpParam*/, 0, NULL);

	//closesocket(pUdpInfo->socket);
	//WSACleanup();

	CloseHandle(hThread);
}

//Received message processing
Msg processMessage(Msg IncMessage)
{
	eMessageType MsgType;
	MsgType = IncMessage.msgType;

	Msg Response;
	ZeroMemory(&Response, sizeof(Response));

	switch (MsgType)
	{
		case PING:
		case QUERY_FIND_NODE:
		case QUERY_FIND_FILE:
			{
				string str, path;
				str = (char *)IncMessage.buffer;
				cout << "FIND FILE Query " << str << " Received from " << IncMessage.fromNode.getNodeIP() << endl;

				Response.msgType = RESPONSE_FIND_FILE;

				if (g_FSM.HasFile(str, path))
				{
					cout << "Found file " << path << endl;
					cout << "Now transferring file to " << IncMessage.fromNode.getNodeIP() << endl;
					// Fills Response with file byte array;
					
					Response.fromNode = g_myNode;
				}
				else 
				{
					cout << "cannot find file " << str << endl;
					cout << "return my closest node in the message buffer" << endl;

					Response.fromNode = g_myNode;
				}
				break;
			}
	}

	return Response;
}

UINT CALLBACK UDPListener(LPVOID lpParam)
{
	LPUDPINFO s = (LPUDPINFO) lpParam;

	SOCKADDR_IN ServerInfo = s->info;
	SOCKADDR_IN FromClient = s->recvInfo;
	SOCKET ServerSocket = s->socket;

	int fromClientSize;

	int recvSize = 0;
	int sendSize = 0;
	
	wcout << L"Opened Server. Listening on port " << PORT_NO << endl;
	
	while(1)
	{
		char buf[1024];

		fromClientSize = sizeof(FromClient);
		recvSize = recvfrom( ServerSocket, buf, BUFFER_SIZE, 0, 
			(struct sockaddr*) &FromClient, &fromClientSize );

		if (recvSize > 0)
		{
			FIND_FILE_MSG *ms = reinterpret_cast<FIND_FILE_MSG*>(buf);
			
			FIND_FILE_MSG response;
			ZeroMemory(&response, sizeof(response));

			//add to routing table
			string newIp = ms->senderIP;
			g_RM.AddNodeToBucket(newIp);

			if (ms->bJoinNetwork)
			{
				cout << ms->senderIP << " has joined the network." << endl;
				response.bJoinNetwork = TRUE;	
			}
			else
			{
				cout << "Message(" << ms->msgID << ") "
					<< "incomming from " << ms->senderIP  
					<< " Requesting " << ms->filename << endl;

				string str = ms->filename;
				string str2;

				response.bJoinNetwork = FALSE;

				//if (g_RM.InsertMarked(ms->msgID))
				//{
					if (g_FSM.HasFile(str, str2))
					{
						//change the bFound flag
						response.nFound = TRUE;
						response.retNode = g_myNode;
					}
					else 
					{
						//send closest node in the FIND_FILE_MSG
						if (g_RM.checkSize())
						{
							//SHA1 sha1;
							//string strHash = sha1(ms->senderIP);

							response.retNode = g_RM.getClosestNode(ms->msgID);
							g_RM.InsertMarked(ms->msgID, response.retNode);
							response.nFound = FALSE;
						}
						else 
						{
							response.nFound = -1;
						}

					}
				//}
				//else
				//{
				//	response.nFound = -1;
				//}
			}
			
			char *tmp = reinterpret_cast<char*>(&response);

			sendSize = sendto( ServerSocket, tmp, sizeof(response), 0, 
				(struct sockaddr*) &FromClient, sizeof(FromClient));
			
		}
	}

	return 0;
}
