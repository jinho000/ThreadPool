#pragma once
#include <Windows.h>
#include <queue>
#include <mutex>
#include <functional>
#include <vector>
#include <thread>
#include <condition_variable>

class ThreadPool
{
private: 
	UINT								m_threadCount;

	std::vector<std::thread>			m_threadArry;
	std::condition_variable				m_conditionalVariable;

	std::queue<std::function<void()>>	m_jobQueue;
	std::mutex							m_jobQueueMutex;

	bool								m_bStopAll;

public: // default
	ThreadPool(UINT _threadCount);
	~ThreadPool();


private:
	static void ThreadWork(ThreadPool& _pThreadPool);

public: // member Func

	// thread pool에 work 전달
	// 모든 스레드 활성화 후 work 실행 (어떤 스레드가 실행중인 상태인지 알기 어려움)
	void AddWork(std::function<void()> _delegateWork);
};

