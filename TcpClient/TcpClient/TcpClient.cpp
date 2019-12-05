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
	addrSrv.sin_addr.S_un.S_addr = inet_addr("127.0.0.1"/*"172.26.65.17"*/);
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(18866);
	connect(sockClient, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR));

	//std::string sendData = "-{\"Cmd\":\"getGamingPlayerSumReq\", \"UserId\":1234,\"SequenceId\":11212312}--{\"Cmd\":\"startGameNotify\",\"GameId\":0,\"UserId\":1234,\"SequenceId\":11212312}--{\"Cmd\":\"startGameNotify\",\"GameId\":0,\"UserId\":1234,\"SequenceId\":11212312}-";
	std::string sendData = "-{\"cmd\":\"getGamingPlayerSumReq\", \"userId\":\"1234\",\"sequenceId\":11212312}-";
	int nRet = send(sockClient, sendData.c_str(), sendData.size(), 0);

	sendData = "-{\"cmd\":\"startGameReq\",\"gameId\":0, \"userId\":\"1234\"}-";
	nRet = send(sockClient, sendData.c_str(), sendData.size(), 0);

	//3. 开始识别人脸
	sendData = "-{\"cmd\":\"startScanFaceReq\",\"userId\":\"1234\"}-";
	nRet = send(sockClient, sendData.c_str(), sendData.size(), 0);

	//4.识别人脸结束
	sendData = "-{\"cmd\":\"endScanFaceReq\",\"userId\":\"1234\",\"code\":0}-"; //0为识别人脸成功，-1为识别人脸超时
	nRet = send(sockClient, sendData.c_str(), sendData.size(), 0);

	//5.所有人都完成了人脸识别通知
	//sendData = "-{ "Cmd":"allFinishScanFaceNotify","Code" : 0 }//0为所有人识别人脸成功，-1为其中有一人识别人脸超时

	//6.2P手机申请加入游戏
	sendData = "-{\"cmd\":\"joinGameReq\",\"userId\":\"1234\"}-";
	//nRet = send(sockClient, sendData.c_str(), sendData.size(), 0);

	//7.2P加入游戏通知1P手机中断游戏重新开始人脸识别
	sendData = "-{\"cmd\":\"joinGameReq\" }-";
	//nRet = send(sockClient, sendData.c_str(), sendData.size(), 0);

	//8.游戏结束通知
	sendData = "-{\"cmd\":\"gameStopReq\",\"userId\":\"1234\",\"score\":100 }-"; //"Score"为得分
	nRet = send(sockClient, sendData.c_str(), sendData.size(), 0);

	//9. 游戏结束得分通知双方手机
	//sendData = "-{\"Cmd\":\"gameScoreReq\",\"UserId\":\"1234\",\"OthersScore\":100 }-";
	//nRet = send(sockClient, sendData.c_str(), sendData.size(), 0);

	char recvBuf[1024] = { 0 };
	nRet = recv(sockClient, recvBuf, 1024, 0);
	if (nRet > 0)
	{
		recvBuf[nRet] = '\0';
		std::cout << "Recv server data=" << recvBuf << std::endl;
	}

	closesocket(sockClient);
	WSACleanup();
}
