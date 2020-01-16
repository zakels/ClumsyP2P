#include "Node.h"
#include <vector>

using namespace std;

const int BUCKETSIZE = 16; // IDEAL VALUE 16

class Bucket
{
public:
	Bucket();
	~Bucket();
private:
	int m_BucketSize;
	int m_nPrefixLength;				// each buckets will have specifier
	vector<Node> m_Bucket;				// old node at top, fresh nodes at the tail
	//DWORD
public:
	vector<Node> getBucket() { return m_Bucket; }
	int getSize(){ return m_BucketSize; }
	Node GetTopNode(vector<string>markedNodes);
	BOOL AddNode(Node node);			
	void RemoveNode(Node node);


	void Print();
};