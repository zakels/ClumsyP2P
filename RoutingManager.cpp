#include "stdafx.h"
#include "Node.h"
#include "Client.h"
#include "Server.h"
#include "sha1.h"
#include "FileStorageManager.h"
#include "RoutingManager.h"
#include <ctime>
#include <bitset>
#include <iostream>

extern Node g_myNode;
extern FileStorageManager g_FSM;

RoutingManager::RoutingManager()
{
	// RULES 
	//
	// - Each Bucket in the RoutingTable will populate until BUCKSIZE(16)
	//
	// - Bucket will split when it goes over 17
	//		* Spliting process will be sorting Nodes based on their closeness (to myNode)
	//
	// - Bucket will not split when k > KBUCKETS(20)
	//
	//
}

RoutingManager::~RoutingManager(){}

//Initializing Manager
void RoutingManager::Init(BOOL bBootstrap, Node mySelf)
{
	myNode = mySelf;
	// Load initial Bucket
	Bucket b_init;
	m_routingTable.push_back(b_init);
	m_nBuckets++;
	if (bBootstrap)
	{
		// Open up Server and just listen
		CreateServer(mySelf.getNodeIP().c_str());
	}
	else
	{	
		// Add Bootstrapping Node and ping the node (Initializing)
		Node n("192.168.21.140");
		m_routingTable.front().AddNode(n);

		// Let Bootstrapping Node know, myNode joined the network
		sendFindFileMessage("", n, "", TRUE);

		CreateServer(mySelf.getNodeIP().c_str());
	}
}

BOOL RoutingManager::Ping(Node target)
{
	Msg response;
	response = ping(target, g_myNode);
	
	if (response.msgType == PONG)
		return TRUE;

	return false;
}

Node RoutingManager::FindNode(Node n)
{
	return n;
}

// Only Sending
void RoutingManager::FindFile(string filename)
{
	Node closestNode = getFirstNode();
	cout << "Closest Node is " << closestNode.getNodeIP() << endl;

	FIND_FILE_MSG response;

	//Generate MessageID on the first loop-back
	time_t rawTime;
	struct tm* timeinfo;
	char bf[80];
	time(&rawTime);
	timeinfo = localtime(&rawTime);
	strftime(bf, sizeof(bf), "%d-%m-%Y %H:%M:%S", timeinfo);
	string strMsgID(bf);


	// enter loop
	while(1)
	{
		response = sendFindFileMessage(filename, closestNode, strMsgID, FALSE);

		if (response.nFound == 1)
		{
			cout << "File found at the node(" << response.retNode.getNodeIP() <<")" << endl;
			break;
		}
		else if (response.nFound == 0)
		{
			closestNode = response.retNode;
			AddNodeToBucket(response.retNode.getNodeIP());
		}
		else 
		{
			cout << "Unable to find the file in the network" << response.retNode.getNodeIP() << endl;
			break;
		}
	}
}

// automatically finds appropriate bucket to insert the node
void RoutingManager::AddNodeToBucket(string ip)
{
	Node newNode(ip);
	// distance becomes the index
	int distance = GetLengthCommonPrefix(g_myNode, newNode);

	int index = 0;
	(m_nBuckets >= distance) ? index = distance : index = m_nBuckets - 1;
	m_routingTable.at(index).AddNode(newNode);
}

Node RoutingManager::getFirstNode()
{
	return m_routingTable.back().getBucket().at(0);
}

void RoutingManager::SendFile(Node n)
{
	
}

void RoutingManager::SplitBuckets()
{
	//if (m_routingTable.back().getSize() > BUCKETSIZE)
	//{
	m_nBuckets++;
	//}
}

// returns false when msg has already been processed in the server
BOOL RoutingManager::InsertMarked(string msgID, Node n)
{
	//pair<set<string>::iterator, bool > Result;
	//Result = m_Marked.insert(msgID);
	//if( Result.second )
	//{
	//	bRet = TRUE;
	//}
	//else
	//{
	//	bRet = FALSE;
	//}	
	string nodeToCheck = n.getHashInString();

	vector<string> visitedNodes;
	visitedNodes.push_back(nodeToCheck);
	visitedNodes.push_back(g_myNode.getHashInString());
	//MsgID 없으면 맵에 넣고 있으면 해당 msgID의 vector를 꺼내와서 안에 체크하고자 하는 노드해시가 있으면 다른 값 없으면 더해준다. 
	pair<map<string, vector<string>>::iterator,bool> Result;
	Result = m_Marked.insert(make_pair(msgID, visitedNodes));
	if ( !Result.second )
	{
		//MsgID 있음
		visitedNodes = m_Marked.find(msgID)->second;
		visitedNodes.push_back(msgID);
		
		map<string, vector<string>>::iterator it;
		it = m_Marked.find(msgID);
		
		m_Marked.erase(it);
		
		m_Marked.insert(make_pair(msgID, visitedNodes));
	}

	map<string, vector<string>>::iterator it = m_Marked.begin();
	for (it; it != m_Marked.end(); ++it)
	{
		cout << "Msg: " << (*it).first << endl;
		for (vector<string>::iterator it2 = (*it).second.begin(); it2 != (*it).second.end(); ++it2)
		{
			cout << "Marked Node : " << (*it2) << "  ";
		}
		cout << endl;
	}
	

	//m_Marked.insert(make_pair(msgID))
	return TRUE;
}

// General XOR Compution(Same bit 0 // Otherwise 1) for logical distance.
// Lower the value, shorter the distance
XOR_DISTANCE RoutingManager::ComputeLogicalDistance(Node n1, Node n2)
{
	XOR_DISTANCE xor;

	xor.dwFirst = n1.getFirstByte() ^ n2.getFirstByte();	//dwFirst = 0x39128392
	xor.dwSecond = n1.getSecondByte() ^ n2.getSecondByte();
	xor.dwThird = n1.getThirdByte() ^ n2.getThirdByte();
	xor.dwFourth = n1.getFourthByte() ^ n2.getFourthByte();
	xor.dwFifth = n1.getFifthByte() ^ n2.getFifthByte();

	return xor;
}
BOOL RoutingManager::checkSize()
{
	BOOL bRet = FALSE;
	
	if (m_routingTable.back().getSize() > 0)
	{
		 bRet = TRUE;
	}
	
	cout << "checkSize() BucketSize(" << m_routingTable.back().getSize() << ")" <<  " Return(" << bRet << ")" << endl;

	return bRet;
}

Node RoutingManager::getClosestNode(string msgID)
{
	// closest node & active node exists at the top position of tail bucket of the routing table
	map<string,vector<string>>::iterator it;
	
	it = m_Marked.find(msgID);

	if (it == m_Marked.end())
	{
		return getFirstNode();
	}
	vector<string> markedNodes = m_Marked.find(msgID)->second;
	return m_routingTable.back().GetTopNode(markedNodes);
}

// Max can be 159 (one bit difference) 
// first bit to be 0 is the length of prefix, more consecutive 0s longer the prefix
size_t RoutingManager::GetLengthCommonPrefix(Node n1, Node n2)
{
	XOR_DISTANCE xor;
	xor = ComputeLogicalDistance(n1, n2);

	bitset<32> s1((unsigned long) xor.dwFirst);
	bitset<32> s2((unsigned long) xor.dwSecond);
	bitset<32> s3((unsigned long) xor.dwThird);
	bitset<32> s4((unsigned long) xor.dwFourth);
	bitset<32> s5((unsigned long) xor.dwFifth);

	string str(s1.to_string());
	str += s2.to_string();
	str += s3.to_string();
	str += s4.to_string();
	str += s5.to_string();

	//cout << str << endl;

	// find 1 then the prefix iteration is over
	// assume there is at least one '1' (no hash collision)
	size_t prefixLen = str.find('1');
	if (prefixLen != 0)
		prefixLen--;

	return prefixLen;
}

void RoutingManager::Print()
{
	cout << "================Table Iteration Start================" << endl;
	int count = 1;
	for (vector<Bucket>::iterator it = m_routingTable.begin(); it != m_routingTable.end(); ++it)
	{
		
		cout << "Bucket #" << count++ << endl;
		(*it).Print();
		
	}
	cout << "=================Table Iteration End=================" << endl;
}