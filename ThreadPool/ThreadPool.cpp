#include "ThreadPool.h"
#include <iostream>
#include <string>
#include <Windows.h>

using namespace std;

#define BIND_FUNC(fn) [fn](){fn();} 
ThreadPool::ThreadPool(UINT _threadCount)
	: m_threadCount(_threadCount)
	, m_bStopAll(false)
{
	for (int i = 0; i < m_threadCount; ++i)
	{
		m_threadArry.push_back(std::thread([this]() {this->ThreadWork(*this); }));
	}

}

ThreadPool::~ThreadPool()
{
	m_bStopAll = true;
	m_conditionalVariable.notify_all();

	for (int i = 0; i < m_threadCount; ++i)
	{
		m_threadArry[i].join();
	}
}


void ThreadPool::ThreadWork(ThreadPool& _pThreadPool)
{
	while (true)
	{
		std::unique_lock<std::mutex> lock(_pThreadPool.m_jobQueueMutex);
		_pThreadPool.m_conditionalVariable.wait(lock, [&_pThreadPool]() {
			return _pThreadPool.m_jobQueue.empty() == false || _pThreadPool.m_bStopAll == true; });

		if (_pThreadPool.m_bStopAll == true && _pThreadPool.m_jobQueue.empty() == true)
		{
			return;
		}

		std::function<void()> job;
		{
			if (_pThreadPool.m_jobQueue.empty() == true)
				return;

			job = std::move(_pThreadPool.m_jobQueue.front());
			_pThreadPool.m_jobQueue.pop();
		}

		lock.unlock();

		if (job != nullptr)
		{
			job();
		}
	}
}



// 템플릿 가변인자 바꾸기
void ThreadPool::AddWork(std::function<void()> _delegateWork)
{
	// produce
	{
		std::unique_lock<std::mutex> lock(m_jobQueueMutex);

		m_jobQueue.push(_delegateWork);
	}

	m_conditionalVariable.notify_one();
}

