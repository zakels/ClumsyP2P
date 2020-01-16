#pragma once

#include "Message.h"

Msg ping(Node targetNode, Node me);
//Node findFile(string filename, Node targetNode, Node me);
FIND_FILE_MSG sendFindFileMessage(string filename, Node targetNode, string MsgID, BOOL bJoin);

