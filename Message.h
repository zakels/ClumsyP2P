#pragma once

#include "define.h"
#include "Server.h"
#include "Node.h"
#include <WinInet.h>
#include <Ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

enum eMessageType
{
	PING,						// Query    : Byte buffer is filled with "hi" char *
	PONG,						// Response : Byte buffer is filled with "ho" char *
	QUERY_FIND_NODE,			// Query    : Byte buffer is filled with Target Node
	RESPONSE_FIND_NODE,			// Response : Byte buffer is filled with closest Node
	QUERY_FIND_FILE,			// Query    : Byte buffer is filled with Target File Name
	RESPONSE_FIND_FILE,			// Response : Byte buffer is filled with closest Node / Target File Path 
};

//enum eQueryType
//{
//
//};
//
//enum eResponseType
//{
//	PONG,
//};

typedef struct _msg
{
	eMessageType msgType;
	BYTE * buffer;				// Differs by message Type
	Node fromNode;				// msg sender
}MESSAGE, Msg, *LPMESSAGE;


// Message ID behaves like query ID. It stands unique for each file request. 

typedef struct _find_file_msg
{	
	CHAR msgID[160];		// MessageID - SHA1 Hash of currentTime
	BOOL bJoinNetwork;		// 1 if it is just joining // 0 for File Query
	int nFound;				// 0 if not found, 1 if found, -1 cannot be reached
	Node retNode;			// returns closest node if not found, if found return itself.
	CHAR senderIP[32];		
	CHAR filename[64];
}FIND_FILE_MSG;
