#include "ThreadPool.h"
#include <iostream>
#include <string>
#include <Windows.h>

using namespace std;

ThreadPool::ThreadPool(UINT _threadCount)
	: m_threadCount(_threadCount)
	, m_bStopAll(false)
{
	for (int i = 0; i < m_threadCount; ++i)
	{
		m_threadArry.push_back(std::thread(&ThreadPool::ThreadWork, this));
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


void ThreadPool::ThreadWork()
{
	while (true)
	{
		std::unique_lock<std::mutex> lock(m_jobQueueMutex);

		// job이 올때까지 대기
		// 스레드 종료시 마지막 남은 작업까지 완료 후 종료한다
		m_conditionalVariable.wait(lock, [this]() {
			return this->m_jobQueue.empty() == false || this->m_bStopAll == true;
		});

		// 종료조건 처리
		if (m_bStopAll == true && m_jobQueue.empty() == true)
			return;
		
		std::function<void()> job = std::move(m_jobQueue.front());
		m_jobQueue.pop();
		
		lock.unlock();

		job();
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

