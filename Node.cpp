#include "stdafx.h"
#include <iostream>
#include "Node.h"
#include "sha1.h"
#include <sstream>
#include <Iphlpapi.h>
#include <algorithm>


#pragma comment(lib, "iphlpapi.lib")

using namespace std;

string _getLocalAddress();

Node::Node()
{
	m_IP = _getLocalAddress();
	m_port = PORT_NO;
	GenerateNodeKey();
}
Node::Node(BOOL bEmpty)
{

}

Node::Node(string ip)
{
	m_IP = ip;
	m_port = PORT_NO;
	GenerateNodeKey();
}

Node::~Node(){}

XOR_DISTANCE Node::operator ^(Node const &obj)
{
	// to be implemented
	XOR_DISTANCE xor;
	return xor;
}

BOOL Node::operator <(Node const &obj)
{
	// to be implemented
	return 0;
}

BOOL Node::operator==(Node const &obj)
{
	BOOL bRet = FALSE;
	if (getFirstByte() == obj.m_nodeID.dwFirst)
	{
		if (getSecondByte() == obj.m_nodeID.dwSecond)
		{
			if (getThirdByte() == obj.m_nodeID.dwThird)
			{
				if (getFourthByte() == obj.m_nodeID.dwFourth)
				{
					if (getFifthByte() == obj.m_nodeID.dwFifth)
					{
						return TRUE;
					}
				}
			}
		}
	}
	return bRet;
}


void Node::GenerateNodeKey()
{
	// Note that sha1() should be modified to assign m_NodeID in the future

	string str = m_IP;
	
	str.erase(remove(str.begin(), str.end(), '.'), str.end());

	SHA1 sha1;
	string strHash = sha1(str);

	m_strNodeID = strHash;

	int len = strlen(strHash.c_str());
	int offset = len / 5;

	SHA1_HASH node_key;

	istringstream ss1(strHash.substr(0,8).c_str()); // first 8 bytes (64bits)
	ss1 >> hex >> node_key.dwFirst;
	istringstream ss2(strHash.substr(8,8).c_str());
	ss2 >> hex >> node_key.dwSecond;
	istringstream ss3(strHash.substr(16,8).c_str());
	ss3 >> hex >> node_key.dwThird;
	istringstream ss4(strHash.substr(24,8).c_str());
	ss4 >> hex >> node_key.dwFourth;
	istringstream ss5(strHash.substr(32,8).c_str());
	ss5 >> hex >> node_key.dwFifth;

	m_nodeID = node_key;
}

void Node::Print()
{
	wcout << hex 
		  << "Node ID : " << m_nodeID.dwFirst << m_nodeID.dwSecond << m_nodeID.dwThird << m_nodeID.dwFourth << m_nodeID.dwFifth 
		  << dec
		  << endl;

	//cout << "(string) : " << m_strNodeID << endl;

	cout << "IP Addr : " << m_IP << endl
		  << "Port	 : " << m_port << endl;
}

string _getLocalAddress()
{
	string str;

	DWORD dwStatus; 
	PIP_ADAPTER_INFO pAdapterInfo = (IP_ADAPTER_INFO *) malloc(sizeof(IP_ADAPTER_INFO));
	ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);
	INT nRetry = 2;
	while (nRetry > 0) 
	{
		dwStatus = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen);

		if (dwStatus == ERROR_BUFFER_OVERFLOW)
		{
			free(pAdapterInfo);
			pAdapterInfo = (IP_ADAPTER_INFO *) malloc(ulOutBufLen);
		}
		else if (dwStatus == ERROR_SUCCESS)
		{
			break;
		}

		nRetry--;
	}

	if (dwStatus == ERROR_SUCCESS)
	{
		// Needing only the first adapter on the list
		string s = pAdapterInfo->IpAddressList.IpAddress.String;
		str = s;
	}
	else 
	{
		str = "";
	}

	free(pAdapterInfo);

	return str;
}