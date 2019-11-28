// TcpClient.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>
#include <Winsock2.h>
#include <stdio.h>
#include <string>
#pragma comment(lib,"ws2_32.lib")

int main()
{
	WSADATA wsaData = { 0 };
	int err = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (err != 0) 
	{
		return 1;
	}

	SOCKET sockClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	SOCKADDR_IN addrSrv;
	addrSrv.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(8866);
	connect(sockClient, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR));

	std::string sendData = "I coming!!!!!";
	int nRet = send(sockClient, sendData.c_str(), sendData.size(), 0);

	char recvBuf[256] = { 0 };
	nRet = recv(sockClient, recvBuf, 256, 0);
	if (nRet > 0)
	{
		recvBuf[nRet] = '\0';
		std::cout << "Recv server data=" << recvBuf << std::endl;
	}

	closesocket(sockClient);
	WSACleanup();
}
