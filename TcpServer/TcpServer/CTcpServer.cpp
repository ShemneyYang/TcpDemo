#include "pch.h"
#include "CTcpServer.h"

CTcpServer::CTcpServer()
	:m_nPort(0)
	,m_callback(NULL)
	,m_hExitEvent(NULL)
{
	_init();
}


CTcpServer::~CTcpServer()
{
}

bool CTcpServer::start(int nPort)
{
	m_nPort = nPort;

	m_hExitEvent = ::CreateEvent(NULL, TRUE, FALSE, L"Exit");
	m_sockSrv = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	do 
	{
		SOCKADDR_IN addrSrv;
		addrSrv.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
		addrSrv.sin_family = AF_INET;
		addrSrv.sin_port = htons(m_nPort);

		//∞Û∂®
		if (SOCKET_ERROR == bind(m_sockSrv, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR)))
		{
			_log("Error!! Failed to bind, err=", WSAGetLastError());
			break;
		}

		//…Ë÷√º‡Ã˝
		if (SOCKET_ERROR == listen(m_sockSrv, 5))
		{
			_log("Error!! Failed to listen, err=", WSAGetLastError());
			break;
		}

		unsigned int nThreadId = 0;
		HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, serverThreadProc, (void*)this, 0, &nThreadId);
		return  true;

	} while (false);
	closesocket(m_sockSrv);
	m_sockSrv = 0;
	return false;
}

bool CTcpServer::stop(void)
{
	if (!m_clientSocketes.empty())
	{
		m_csClientSocketes.lock();
		while (!m_clientSocketes.empty())
		{
			std::map<std::string, SOCKET>::iterator it = m_clientSocketes.begin();
			closesocket(it->second);
			m_clientSocketes.erase(it);
		}
		m_csClientSocketes.unlock();
	}
	if (NULL != m_hExitEvent)
	{
		SetEvent(m_hExitEvent);
	}
	closesocket(m_sockSrv);
	m_sockSrv = 0;
	return true;
}

void CTcpServer::wait(void)
{
	WaitForSingleObject(m_hExitEvent, INFINITE);
}

bool CTcpServer::sendData(const std::string& strIP, const std::string& strData)
{
	SOCKET socket = 0;
	
	{
		AutoCSLocker lock(&m_csClientSocketes);
		std::map<std::string, SOCKET>::iterator it = m_clientSocketes.find(strIP);
		if (it == m_clientSocketes.end())
		{
			return false;
		}
		socket = it->second;
	}

	int nRet = send(socket, strData.c_str(), strData.size(), 0);
	if (nRet > 0)
	{
		return true;
	}
	
	std::string str = "Error!! Failed to send, ip=" + strIP;
	str += ",err=";
	_log(str, WSAGetLastError());
	return false;
}

void CTcpServer::setCallback(CTcpServerCallback* callback)
{
	m_callback = callback;
}

unsigned __stdcall CTcpServer::serverThreadProc(void* lpParam)
{
	std::cout << "Server Thread created, id=" << GetCurrentThreadId() << endl;
	CTcpServer* _this = (CTcpServer*)lpParam;
	_this->_onServerRuning();
	std::cout << "Server Thread exited, id=" << GetCurrentThreadId() << endl;
	return 0;
}

unsigned __stdcall CTcpServer::clientThreadProc(void* lpParam)
{
	ThreadInfo* pThreadInfo = (ThreadInfo*)lpParam;
	std::string strIP = pThreadInfo->strIP;
	std::cout << "Client Thread created, id=" << GetCurrentThreadId() << ",ip=" << strIP.c_str() <<endl;
	pThreadInfo->_this->_onClientRuning(pThreadInfo);
	delete pThreadInfo;
	std::cout << "Client Thread exited, id=" << GetCurrentThreadId() << ",ip=" << strIP.c_str() << endl;
	return 0;
}

void CTcpServer::_onServerRuning(void)
{
	SOCKADDR_IN addrClient;
	int len = sizeof(SOCKADDR);

	_log("Start TCP accept!");
	while (WaitForSingleObject(m_hExitEvent, 0) == WAIT_TIMEOUT)
	{
		SOCKET sockConn = accept(m_sockSrv, (SOCKADDR*)&addrClient, &len);

		char cIP[128] = { 0 };
		sprintf_s(cIP, 128, "%s:%d", inet_ntoa(addrClient.sin_addr), ntohs(addrClient.sin_port));
		//std::cout << getSysTime().c_str() << ">>IP=" << cBuf << " connected!" << std::endl;
		std::string str = "IP=";
		str += cIP;
		str += " connected!";
		_log(str);
		if (NULL != m_callback)
		{
			m_callback->onIPConnected(cIP);
		}

		ThreadInfo* lpParam = new ThreadInfo();
		lpParam->socket = sockConn;
		lpParam->strIP = cIP;
		lpParam->_this = this;
		unsigned int nThreadId = 0;
		HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, clientThreadProc, (void*)lpParam, 0, &nThreadId);

		AutoCSLocker locker(&m_csClientSocketes);
		m_clientSocketes.insert(std::make_pair(lpParam->strIP, lpParam->socket));
	}
}

void CTcpServer::_onClientRuning(ThreadInfo* info)
{
	SOCKET socket = info->socket;
	char recvBuf[65536/*1024 * 64*/] = { 0 };

	bool bExit = false;
	while (!bExit)
	{
		int nRet = recv(socket, recvBuf, 65536, 0);
		if (nRet > 0)
		{
			recvBuf[nRet] = '\0';
			if (NULL != m_callback)
			{
				m_callback->onRecvData(info->strIP, std::string(recvBuf, nRet));
			}
		}
		else if (nRet == 0)
		{
			std::string str = "IP=" + info->strIP + " connection closed!";
			_log(str);
			bExit = true;
		}
		else
		{
			std::string str = "Error!! Failed to recv, IP=" + info->strIP + ",err=";
			_log(str, WSAGetLastError());
			bExit = true;
		}
	}
	closesocket(socket);

	_remvoeConnection(info->strIP);
}

bool CTcpServer::_init(void)
{
	WSADATA wsaData = { 0 };
	int err = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (err != NO_ERROR)
	{
		_log("Error!! Failed to WSAStartup, err=", WSAGetLastError());
		return false;
	}
	return true;
}

bool CTcpServer::_remvoeConnection(const std::string& strIP)
{
	m_csClientSocketes.lock();
	std::map<std::string, SOCKET>::iterator it = m_clientSocketes.find(strIP);
	if (it == m_clientSocketes.end())
	{
		m_csClientSocketes.unlock();
		return false;
	}
	m_csClientSocketes.unlock();
	if (NULL != m_callback)
	{
		m_callback->onIPConnectionClosed(strIP);
	}
	return false;
}

void CTcpServer::_log(const std::string& strLog)
{
	if (NULL == m_callback)
	{
		return;
	}
	m_callback->onLog(strLog);
}

void CTcpServer::_log(std::string strData, int nErr)
{
	char cBuffer[32] = { 0 };
	_itoa_s(nErr, cBuffer, 31, 10);
	strData += cBuffer;
	_log(strData);
}