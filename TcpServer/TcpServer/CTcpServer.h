#pragma once

#include <Winsock2.h>
#include <Windows.h>
#include <iostream>
#include <process.h>
#include <string>
#include <map>

#include "CLocker.h"
#include "CTcpServerCallback.h"

using namespace std;

class CTcpServer
{
public:
	typedef struct SThreadInfo
	{
		SOCKET socket;
		std::string strIP;
		CTcpServer* _this;

		SThreadInfo() :socket(0), _this(NULL) {};
	}ThreadInfo;

public:
	CTcpServer();
	~CTcpServer();

	bool start(int nPort);

	bool stop(void);

	void wait(void);

	bool sendData(const std::string& strIP, const std::string& strData);

	void setCallback(CTcpServerCallback* callback);

protected:
	static unsigned __stdcall serverThreadProc(void* lpParam);
	static unsigned __stdcall clientThreadProc(void* lpParam);
	void _onServerRuning(void);
	void _onClientRuning(ThreadInfo* info);

private:
	bool _init(void);
	bool _remvoeConnection(const std::string& strIP);
	void _log(const std::string& strLog);
	void _log(std::string strData, int nErr);

private:
	int m_nPort;
	SOCKET m_sockSrv;
	HANDLE m_hThreadSrv;
	HANDLE m_hExitEvent;

	CSLock m_csClientSocketes;
	std::map<std::string, SOCKET> m_clientSocketes;

	CTcpServerCallback* m_callback;
};

