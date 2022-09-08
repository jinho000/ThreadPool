#pragma once
#include <Windows.h>
#include <queue>
#include <mutex>
#include <functional>
#include <vector>


class ThreadPool
{
private: 
	std::vector<HANDLE> m_eventHandleArry;
	std::vector<HANDLE> m_threadHandleArry;
	std::vector<DWORD>  m_threadIDArry;
	UINT				m_threadCount;
	
	std::queue<std::function<void()>>	m_jobQueue;
	std::mutex							m_jobQueueMutex;

	bool								m_bDestroyThread;
	std::mutex							m_bDestroyMutex;
public: // default
	ThreadPool(UINT _threadCount);
	~ThreadPool();


private:
	static DWORD WINAPI WorkThreadFunc(LPVOID lpParameter);

	void WorkJob();

public: // member Func

	// thread pool에 work 전달
	// 모든 스레드 활성화 후 work 실행 (어떤 스레드가 실행중인 상태인지 알기 어려움)
	void AddWork(std::function<void()> _delegateWork);
};

