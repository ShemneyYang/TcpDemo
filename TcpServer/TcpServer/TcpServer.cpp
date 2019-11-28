// TcpServer.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>
#include <Winsock2.h>
#include <stdio.h>
#include <set>
#include <string>
#include <Windows.h>
#include <process.h>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

struct SThreadInfo
{
	SOCKET socket;
	std::string strIP;
};

unsigned __stdcall threadProc(LPVOID lpParam);

int main()
{
	std::cout << "Start TCP Server!!!!!!!!" << endl;
	WSADATA wsaData = { 0 };
	int err = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (err != NO_ERROR) 
	{
		std::cout << "Error!! Failed to WSAStartup, err=" << WSAGetLastError() << endl;
		return 1;
	}

	SOCKET sockSrv = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	SOCKADDR_IN addrSrv;
	addrSrv.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(8866);

	if (SOCKET_ERROR == bind(sockSrv, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR)))
	{
		std::cout << "Error!! Failed to bind, err=" << WSAGetLastError() << endl;
		return 1;
	}

	if (SOCKET_ERROR == listen(sockSrv, 5))
	{
		std::cout << "Error!! Failed to listen, err=" << WSAGetLastError() << endl;
		return 1;
	}

	SOCKADDR_IN addrClient;
	int len = sizeof(SOCKADDR);

	std::cout << "Start TCP accept!!!!!!!!" << endl;
	while (true)
	{
		SOCKET sockConn = accept(sockSrv, (SOCKADDR*)&addrClient, &len);
		char cBuf[256] = { 0 };
		sprintf_s(cBuf, 256, "%s", inet_ntoa(addrClient.sin_addr));
		//send(sockConn, sendBuf, strlen(sendBuf) + 1, 0);
		std::cout << "Hello IP=" << cBuf << std::endl;

		SThreadInfo* lpParam = new SThreadInfo();
		lpParam->socket = sockConn;
		lpParam->strIP = cBuf;
		unsigned int nThreadId = 0;
		HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, threadProc, (void*)lpParam, 0, &nThreadId);
	}
}

unsigned __stdcall threadProc(LPVOID lpParam)
{
	SThreadInfo* pThreadInfo = (SThreadInfo*)lpParam;
	SOCKET socket = pThreadInfo->socket;
	char recvBuf[1024] = { 0 };

	bool bExit = false;
	while (!bExit)
	{
		int nRet = recv(socket, recvBuf, 1024, 0);
		if (nRet > 0)
		{
			recvBuf[nRet] = '\0';
			std::cout << "IP:" << pThreadInfo->strIP.c_str() << ",data=" << recvBuf << endl;
			send(socket, recvBuf, strlen(recvBuf) + 1, 0);
		}
		else if (nRet == 0)
		{
			bExit = true;
		}
		else
		{
			std::cout << "Error!! Failed to recv, err=" << WSAGetLastError() << endl;
			bExit = true;
		}
	}
	std::cout << "IP:" << pThreadInfo->strIP.c_str() << " close!!!!!!" << endl;
	closesocket(socket);
	delete pThreadInfo;
	return 0;
}
