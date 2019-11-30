#pragma once

#include <windows.h>

class CSLock
{
public:
	CSLock()
	{
		InitializeCriticalSection(&m_criticalSection);
	}

	~CSLock()
	{
		DeleteCriticalSection(&m_criticalSection);
	}

	void lock()
	{
		EnterCriticalSection(&m_criticalSection);
	}

	void unlock()
	{
		LeaveCriticalSection(&m_criticalSection);
	}

	bool tryLock()
	{
		return TryEnterCriticalSection(&m_criticalSection) ? true : false;
	}

private:
	CRITICAL_SECTION m_criticalSection;
};

class AutoCSLocker
{
public:
	explicit AutoCSLocker(CSLock *m)
		: m_lock(m)
	{
		m_lock->lock();
	}

	~AutoCSLocker()
	{
		m_lock->unlock();
		m_lock = NULL;
	}

private:
	CSLock *m_lock;
};