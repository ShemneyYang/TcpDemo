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
#include <time.h>
#include "CTcpServer.h"
#include "CTcpServerCallback.h"

#pragma comment(lib, "ws2_32.lib")

using namespace std;

std::string getSysTime(void);

class TcpServerApp:public CTcpServerCallback
{
public:
	TcpServerApp() 
	{
		m_tcpServer.setCallback(this);
		m_tcpServer.start(8866);
	};

	virtual ~TcpServerApp() 
	{
		m_tcpServer.setCallback(NULL);
	};

	virtual void onIPConnected(const std::string& strIP)
	{
		std::cout << getSysTime().c_str() << ">>onIPConnected " << strIP.c_str() << endl;
	};

	virtual void onIPConnectionClosed(const std::string& strIP)
	{
		std::cout << getSysTime().c_str() << ">>onIPConnectionClosed " << strIP.c_str() << endl;
	};

	virtual void onRecvData(const std::string& strIP, const std::string& strData)
	{
		std::cout << getSysTime().c_str() << ">>onRecvData IP=" << strIP.c_str() << ",data=" << strData.c_str() << endl;
		if (m_tcpServer.sendData(strIP, strData))
		{
			std::cout << getSysTime().c_str() << ">>sendTo IP=" << strIP.c_str() << ",data=" << strData.c_str() << endl;
		}
		else
		{
			std::cout << getSysTime().c_str() << ">>Error:Failed to sendTo IP=" << strIP.c_str() << ",data=" << strData.c_str() << endl;
		}
	};

	virtual void onLog(const std::string& strLog)
	{
		std::cout << getSysTime().c_str() << ">>" << strLog.c_str() << endl;
	};

	void wait(void)
	{
		m_tcpServer.wait();
	};

private:
	CTcpServer m_tcpServer;
};

int main()
{
	std::cout << getSysTime().c_str() << ">>Start TCP Server!" << endl;
	TcpServerApp app;
	app.wait();
}

std::string getSysTime(void)
{
	struct tm localTime;
    time_t t = 0;
    t = time(NULL);
	localtime_s(&localTime, &t);
	char cBuf[64] = { 0 };
    sprintf_s(cBuf, 64, "%02d:%02d:%02d", localTime.tm_hour, localTime.tm_min, localTime.tm_sec);
	return cBuf;
// 	localTime = gmtime(&t);
//     printf("UTC hour is: %d/n", local->tm_hour);
}