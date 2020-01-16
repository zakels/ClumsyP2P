#include "stdafx.h"
#include "Node.h"
#include "Client.h"
#include "sha1.h"
#include "FileStorageManager.h"
#include <iostream>
#include <process.h>

using namespace std;

extern Node g_myNode;

UINT CALLBACK UDPSender(LPVOID lpParam);

void CreateClient()
{
	HANDLE hThread = (HANDLE) _beginthreadex(NULL, 0, UDPSender, NULL/*lpParam*/, 0, NULL);

	CloseHandle(hThread);
}

UINT CALLBACK UDPSender(LPVOID lpParam)
{
	return 0;
}


Msg ping(Node targetNode, Node me)
{
	Msg msg, response;

	return response; 
}

FIND_FILE_MSG sendFindFileMessage(string filename, Node targetNode, string msgID, BOOL bJoin)
{
	WSADATA WSAData;
	SOCKET client;
	SOCKADDR_IN toServer;
	SOCKADDR_IN fromServer;

	int FromServer_Size;
	int Recv_Size;   
	int Send_Size;

	FIND_FILE_MSG msg;
	ZeroMemory(&msg, sizeof(msg));

	SHA1 sha1;
	string strHash = sha1(msgID);

	strcpy(msg.msgID, strHash.c_str());
	strcpy(msg.filename, filename.c_str());
	strcpy(msg.senderIP, g_myNode.getNodeIP().c_str());
	msg.bJoinNetwork = bJoin;

	string ip = targetNode.getNodeIP();
	int port = targetNode.getNodePort();

	char *tmp = reinterpret_cast<char*>(&msg);

	if (WSAStartup(0x202, &WSAData) != SOCKET_ERROR)
	{
		memset(&toServer, 0, sizeof(toServer));
		memset(&fromServer, 0, sizeof(fromServer));

		toServer.sin_addr.s_addr = inet_addr(ip.c_str());
		toServer.sin_family = AF_INET;
		toServer.sin_port = htons(port);

		client = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (client != INVALID_SOCKET)
		{
			// Send Request to Server
			Send_Size = sendto( client, tmp, sizeof(msg), 0,
				(struct sockaddr*) &toServer, sizeof(toServer));
			if (Send_Size == SOCKET_ERROR)
			{

			}
			
			if (!bJoin)
				cout << "Sending Request to " << ip << ":" << port << " (" << msg.filename << ")"<< endl;

			FromServer_Size = sizeof(fromServer);

			char buf[1024];

			// Receive Response from Server
			Recv_Size = recvfrom( client, buf, BUFFER_SIZE, 0,
				(struct sockaddr*) &fromServer, &FromServer_Size);

			FIND_FILE_MSG *ms = reinterpret_cast<FIND_FILE_MSG*>(buf);
			
			if (ms->bJoinNetwork)
			{
				cout << "Joined a server by Bootstrapping Node" << endl;
			}
			else 
			{
				if (ms->nFound != -1)
				{
					cout << "Got Response from the server bFilefound(" << ms->nFound <<") Returned Node("
						<< ms->retNode.getNodeIP() << ")" << endl;

					Node n(ms->retNode.getNodeIP());

					msg.nFound = ms->nFound;
					msg.retNode = n;
				}
				else 
				{
					cout << "Server returned -1 for the query (File cannot be reached)" << endl;
					msg.nFound = ms->nFound;
				}
			}
		}
		closesocket(client);
	}
	WSACleanup();

	return msg;
}