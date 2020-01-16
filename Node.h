 #pragma once

#include <string>

using namespace std;

typedef struct _SHA1_HASH	// express 160-bit as in DWORD(32-bits)
{
	DWORD dwFirst;
	DWORD dwSecond;
	DWORD dwThird;
	DWORD dwFourth;
	DWORD dwFifth;
}SHA1_HASH, XOR_DISTANCE;

class Node
{
public:
	Node(void);
	Node(BOOL bEmpty);
	Node(string ip);
	~Node();

	// Operation overloading for XOR distance
	XOR_DISTANCE operator ^ (Node const &obj);
	BOOL operator < (Node const &obj);
	BOOL operator==(const Node &obj);

private:
	SHA1_HASH m_nodeID;
	//wstring m_wstrNodeID;
	string m_strNodeID;
	int m_port;
	string m_IP;
public:
	DWORD getFirstByte() { return m_nodeID.dwFirst; }
	DWORD getSecondByte() { return m_nodeID.dwSecond; }
	DWORD getThirdByte() { return m_nodeID.dwThird; }
	DWORD getFourthByte() { return m_nodeID.dwFourth; }
	DWORD getFifthByte() { return m_nodeID.dwFifth; }

	int getNodePort(){ return m_port; }
	string getNodeIP(){ return m_IP; }
	void GenerateNodeKey();
	string getHashInString() { return m_strNodeID; }

	void Print();
};