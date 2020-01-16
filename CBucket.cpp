#include "stdafx.h"
#include "CBucket.h"
#include <algorithm>
#include <iostream>

Bucket::Bucket()
{
	m_BucketSize = 0;
	m_nPrefixLength = -1;	// only initial bucket has -1 length. This changes when bucket splits
}

Bucket::~Bucket(){}


// If the bucket already has a node with the same id or with the same
// endpoint, the new node replaces it and is placed at the front of the list
// (most recently seen node)

BOOL Bucket::AddNode(Node node)
{
	BOOL bNeedToSplit = FALSE;
	int index = -1;
	
	vector<Node>::iterator it = find(m_Bucket.begin(), m_Bucket.end(), node);
	
	if (it != m_Bucket.end())
		index = distance(m_Bucket.begin(), it);

	if (index != -1)
	{
		RemoveNode(*it);
		m_Bucket.push_back(node);
	}
	else
	{
		if (m_BucketSize < BUCKETSIZE)
		{
			//insert to tail
			m_Bucket.push_back(node);
			m_BucketSize++;
		}
		else 
		{
			bNeedToSplit = TRUE;
		}
	}
	return bNeedToSplit;
}

// return node if available, if not return empty node
Node Bucket::GetTopNode(vector<string> markedNodes)
{
	// exclude client nodeID
	//if (strNodeID.compare(m_Bucket.front().getHashInString()) == 0)
	//{
	//	return m_Bucket.at(1);
	//}
	//else
	//{
	//	return m_Bucket.front();
	//}	
	Node n(TRUE);
	
	vector<Node>::iterator it = m_Bucket.begin();
	for (it; it != m_Bucket.end(); ++it)
	{
		string nodeID = (*it).getHashInString();
		if (find(markedNodes.begin(), markedNodes.end(), nodeID) == markedNodes.end())
		{
			n = *it;	
		}
	}

	return n;
}

//
//

void Bucket::RemoveNode(Node node)
{
	m_Bucket.erase(remove(m_Bucket.begin(), m_Bucket.end(), node), m_Bucket.end());
}

void Bucket::Print()
{
	for (vector<Node>::iterator it = m_Bucket.begin(); it != m_Bucket.end(); it++)
	{
		cout << (*it).getHashInString() << endl;
	}
}