#pragma once

#include "CBucket.h"
#include "Node.h"
#include <vector>
#include <set>

using namespace std;

const int KBUCKETS = 20;	// should be bit-wise (sha1 160-bit), but for the test, small value of 3

class RoutingManager
{
public:
	RoutingManager(void);
	~RoutingManager(void);
private:
	int m_nBuckets;
	Node myNode;
	BOOL m_bBootstrappingNode;

	map<string, vector<string>> m_Marked;	//<msgID, vector<Node>>
	vector<Bucket> m_routingTable;			//Would vector be better than array? splits?
public:
	void Init(BOOL bBootstrap, Node myself);

	vector<Bucket> getTable() { return m_routingTable; }

	//IMPLEMENTED
	XOR_DISTANCE ComputeLogicalDistance(Node n1, Node n2);
	size_t GetLengthCommonPrefix(Node n1, Node n2);
	Node getClosestNode(string msgID);
	Node getFirstNode();
	BOOL checkSize();
	void AddNodeToBucket(string ip);
	
	BOOL InsertMarked(string msgID, Node n);

	void Print();

	//TO BE
	void SplitBuckets();

	/////////////////////////////////////////////////////////////////////////////////////
	// Kademlia Protocol
	/////////////////////////////////////////////////////////////////////////////////////

	// IMPLEMENTED
	void FindFile(string filename);
	
	// TO BE
	void SendFile(Node n);
	Node FindNode(Node n);
	BOOL Ping(Node Target);						//Send pings

};