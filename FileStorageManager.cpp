#include "stdafx.h"
#include "FileStorageManager.h"
#include <process.h>
#include <iostream>

const int NUM_SECONDS = 1000;
const int NUM_MINUTES = 60 * NUM_SECONDS;

extern FileStorageManager g_FSM;

UINT CALLBACK FileStorageHandler(LPVOID);

void FileStorageInit()
{	
	HANDLE hThread = (HANDLE) _beginthreadex(NULL, 0, FileStorageHandler, NULL, NULL, 0);
	CloseHandle(hThread);
}

FileStorageManager::FileStorageManager()
{
	string wstrPath = "C:\\TestFile\\";
	string wildcard = "*.*";
	DWORD dwAttr = GetFileAttributesA(wstrPath.c_str());
	if (dwAttr == INVALID_FILE_ATTRIBUTES && !(dwAttr & FILE_ATTRIBUTE_DIRECTORY))
	{
		return;
	}

	string fileSpecifier = wstrPath + wildcard;

	WIN32_FIND_DATAA data;
	HANDLE hFind;
	if ((hFind = FindFirstFileA(fileSpecifier.c_str(), &data)) != INVALID_HANDLE_VALUE)
	{
		while (FindNextFileA(hFind, &data) != 0)
		{
			if (data.cFileName[0] == L'.')
			{

			}
			else
			{
				string fileName = data.cFileName;
				string fullFilepath = wstrPath + data.cFileName;
				InsertFileToList(fileName, fullFilepath);
			}
		}
		FindClose(hFind);
	}
}

FileStorageManager::~FileStorageManager()
{

}

void FileStorageManager::InsertFileToList(string strFileName, string strFullPath)
{
	m_mapSharedList.insert(make_pair(strFileName, strFullPath));
}

BOOL FileStorageManager::HasFile(string strFileName, string &strFullPath)
{
	BOOL bRet = FALSE;

	map<string, string>::iterator it;
	it = m_mapSharedList.find(strFileName);
	if (it != m_mapSharedList.end())
	{
		bRet = TRUE;
		strFullPath = it->second;
	}
	
	return bRet;
}

UINT CALLBACK FileStorageHandler(LPVOID lpParam)
{
	while(1)
	{
		//To-do : check & update file every x minutes
	}

	return 0;
}

void FileStorageManager::Print()
{
	cout << "================File Iteration Start================" << endl;
	for(map<string, string>::iterator it = m_mapSharedList.begin(); it != m_mapSharedList.end(); ++it)
	{
		cout << "<" << it->first << ", " << it->second << ">" << endl;
	}
	cout << "==================File Iteration end================" << endl;
}