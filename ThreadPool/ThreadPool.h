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

	// thread pool�� work ����
	// ��� ������ Ȱ��ȭ �� work ���� (� �����尡 �������� �������� �˱� �����)
	void AddWork(std::function<void()> _delegateWork);
};

