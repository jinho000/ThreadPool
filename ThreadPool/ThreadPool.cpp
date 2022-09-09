#include "ThreadPool.h"
#include <iostream>
#include <string>
#include <Windows.h>

using namespace std;

ThreadPool::ThreadPool(UINT _threadCount)
	: m_threadCount(_threadCount)
	, m_bDestroyThread(false)
{
	if (m_threadCount < 1)
	{
		m_threadCount = 1;
	}

	m_eventHandleArry.resize(m_threadCount);
	m_threadHandleArry.resize(m_threadCount);
	m_threadIDArry.resize(m_threadCount);

	for (int i = 0; i < m_threadCount; ++i)
	{
		m_eventHandleArry[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
		m_threadHandleArry[i] = CreateThread(NULL, 0, ThreadPool::WorkThreadFunc, reinterpret_cast<LPVOID>(this), 0, &m_threadIDArry[i]);
	}
}

ThreadPool::~ThreadPool()
{
	while (true)
	{
		m_jobQueueMutex.lock();
		if (m_jobQueue.empty() == false)
		{
			m_jobQueueMutex.unlock();
			Sleep(1);
		}
		else
		{
			m_jobQueueMutex.unlock();
			break;
		}
	}

	m_bDestroyMutex.lock();
	m_bDestroyThread = true;
	m_bDestroyMutex.unlock();
	
	m_jobQueueMutex.lock();
	for (int i = 0; i < m_threadCount; ++i)
	{
		m_jobQueue.push([]() {});
	}
	m_jobQueueMutex.unlock();

	// 모든 스레드를 깨우기
	for (int i = 0; i < m_threadCount; ++i)
	{
		SetEvent(m_eventHandleArry[i]);
	}

	WaitForMultipleObjects(m_threadCount, m_threadHandleArry.data(), TRUE, INFINITE);

	for (int i = 0; i < m_threadCount; ++i)
	{
		if (nullptr != m_threadHandleArry[i])
		{
			CloseHandle(m_threadHandleArry[i]);
		}

		if (nullptr != m_eventHandleArry[i])
		{
			CloseHandle(m_eventHandleArry[i]);
		}
	}
}

DWORD WINAPI ThreadPool::WorkThreadFunc(LPVOID _threadPoolObjectAddr)
{
	ThreadPool* pThreadPoolObj = reinterpret_cast<ThreadPool*>(_threadPoolObjectAddr);
	while (true)
	{
		DWORD result = WaitForMultipleObjects(pThreadPoolObj->m_threadCount, pThreadPoolObj->m_eventHandleArry.data(), false, INFINITE);
		if (WAIT_FAILED == result)
		{
			DWORD error = GetLastError();
			cout << "Error " + std::to_string(error) + "\n";
		}

		pThreadPoolObj->WorkJob();

		pThreadPoolObj->m_bDestroyMutex.lock();
		if (pThreadPoolObj->m_bDestroyThread == true)
		{
			pThreadPoolObj->m_bDestroyMutex.unlock();
			break;
		}
		pThreadPoolObj->m_bDestroyMutex.unlock();
	}

	return 0;
}

void ThreadPool::WorkJob()
{
	std::function<void()> job;

	{
		std::lock_guard<std::mutex> lock(m_jobQueueMutex);

		if (m_jobQueue.empty() == true)
			return;	
	
		job = std::move(m_jobQueue.front());
		m_jobQueue.pop();
	}

	job();
}

void ThreadPool::AddWork(std::function<void()> _delegateWork)
{
	m_jobQueueMutex.lock();
	m_jobQueue.push(_delegateWork);
	m_jobQueueMutex.unlock();

	// 모든 스레드를 깨워야함(어떤스레드가 실행중이고 아닌지 알 수 없음
	for (int i = 0; i < m_threadCount; ++i)
	{
		SetEvent(m_eventHandleArry[i]);
	}
}

