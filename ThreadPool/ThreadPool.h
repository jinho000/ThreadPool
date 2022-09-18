#pragma once
#include <Windows.h>
#include <queue>
#include <mutex>
#include <functional>
#include <vector>
#include <thread>
#include <condition_variable>
#include <atomic>

#define BIND_FUNC(fn) [](){ fn();} 

class ThreadPool
{
private: 
	std::vector<std::thread>	m_threadArry;

	HANDLE						m_hEmptyCheckSemaphore;
	HANDLE						m_hFullCheckSemaphore;
	HANDLE						m_hPSemaphore;
	HANDLE						m_hCSemaphore;

	UINT						m_threadCount;
	UINT						m_maxJobCount;

	std::function<void()>*		m_jobQueue;
	int							m_bufferInIdx;
	int							m_bufferOutIdx;

	std::atomic<bool>			m_bStopAll;

public: // default
	ThreadPool(UINT _threadCount, UINT _maxJobCount);
	~ThreadPool();

private:
	void ThreadWork();

public: // member Func

	// thread pool¿¡ work Àü´Þ
	void AddWork(std::function<void()> _delegateWork);
};

