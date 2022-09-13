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

	// thread pool�� work ����
	// ��� ������ Ȱ��ȭ �� work ���� (� �����尡 �������� �������� �˱� �����)
	void AddWork(std::function<void()> _delegateWork);
};

