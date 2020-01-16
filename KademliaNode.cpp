#include "stdafx.h"
#include "KademliaNode.h"


KademliaNode::~KademliaNode()
{

}

void KademliaNode::Bootstrap(KademliaNode bsNode)
{
	Update(bsNode);
	FindNode(_id, bsNode);
}

void KademliaNode::Close()
{
	vector<KademliaNode> nodes;
	for (vector<Bucket>::iterator it = _table.begin(); it != _table.end(); it++)
	{
		
	}
}