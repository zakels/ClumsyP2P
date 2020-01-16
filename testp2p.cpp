// testp2p.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "FileStorageManager.h"
#include "RoutingManager.h"
#include "Node.h"
#include "Client.h"
#include <iostream>
#include <fstream>

using namespace std;

FileStorageManager g_FSM; 
RoutingManager g_RM;
Node g_myNode;

// TODOS:
//			Serializing UDP Packets (Right now its just type-casting)
//			Protocol (PING, STORE, FIND_NODE) (Right now it only relies on FIND_VALUE)
//			Transfer files
//			Updating Routing Table
//			Error Handling (Edge cases)

void Print()
{
	g_myNode.Print();
	cout << endl;
	g_RM.Print();
	cout << endl;
	g_FSM.Print();
	cout << endl;
}

int _tmain(int argc, _TCHAR* argv[])
{
	// Steps to follow
	//

	// Creates node (my own)
	//      Needs to get assigned a specific and unique node id
	//		(Bootstrapping Node assigns IDs to nodes when it joins the network)
	//
	// Creates Routing Manager (Thread)
	//		Try to connect to bootstrapping node (Insert bootstrapping node)
	//			if the address is bootstrapping node,itself, just open a connection
	//		Insert Bootstrapping node and populate the list
	//		broadcast / ping neighbors every once in a while
	//
	// Creates Storage Manager (Thread)
	//		Insert shared files in local pc
	//		Check if the file exist every once in a while


	FileStorageInit();

	BOOL bServer = FALSE;
	string modeFile = "client.txt";
	ifstream ifs(modeFile.c_str());

	if (!ifs.is_open())
	{
		bServer = TRUE;
	}

	//wstring str = L"19.168.21.120";
	//Node yourNode(str, 49312);
	//g_RM.GetLengthCommonPrefix(myNode, yourNode);
	g_RM.Init(bServer, g_myNode);

	Sleep(2500);

	g_myNode.Print();
	cout << endl;
	g_RM.Print();
	cout << endl;
	g_FSM.Print();

	cout << endl << endl << endl << "Menu" << endl;
	cout << "1. Search File by file name" << endl;
	cout << "2. View my info" << endl;
	cout << "3. Exit" << endl;

	string filename;

	while (1)
	{	
		int input;
		cout << "Input : ";
		cin >> input;

		switch(input)
		{
			case 1:
				cout << "Enter the name of the file :";  
				cin >> filename;

				g_RM.FindFile(filename);

				break;
			case 2:
				Print();
				break;
			case 3:
				return 0;
				break;

			default:
				break;
		}
	}
}

