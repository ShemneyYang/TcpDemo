#pragma once

#include <string>

//ע���ص����������϶��������̵߳�.
class CTcpServerCallback
{
public:
	CTcpServerCallback(void) {};
	virtual ~CTcpServerCallback(void) {};

	virtual void onIPConnected(const std::string& strIP) = 0;

	virtual void onIPConnectionClosed(const std::string& strIP) = 0;

	virtual void onRecvData(const std::string& strIP, const std::string& strData) = 0;

	virtual void onLog(const std::string& strLog) = 0;
};