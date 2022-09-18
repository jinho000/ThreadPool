#include "ThreadPool.h"
#include <iostream>
#include <string>
#include <cassert>
#include <Windows.h>

using namespace std;

ThreadPool::ThreadPool(UINT _threadCount, UINT _maxJobCount)
	: m_threadCount(_threadCount)
	, m_maxJobCount(_maxJobCount)
	, m_bufferInIdx()
	, m_bufferOutIdx()
{
	m_bStopAll.store(false);

	// 세마포어 초기화
	m_hEmptyCheckSemaphore = CreateSemaphore(NULL, m_maxJobCount, m_maxJobCount, NULL);
	m_hFullCheckSemaphore = CreateSemaphore(NULL, 0, m_maxJobCount, NULL);;

	m_hPSemaphore = CreateSemaphore(NULL, 1, 1, NULL);
	m_hCSemaphore = CreateSemaphore(NULL, 1, 1, NULL);

	// job 큐 생성
	m_jobQueue = new std::function<void()>[m_maxJobCount];

	// 스레드 생성
	for (int i = 0; i < m_threadCount; ++i)
	{
		m_threadArry.push_back(std::thread(&ThreadPool::ThreadWork, this));
	}
}

ThreadPool::~ThreadPool()
{
	// 종료시키기위해 job을 넣어줌
	m_bStopAll.store(true);

	for (int i = 0; i < m_threadCount; ++i)
	{
		AddWork([]() {});
	}

	for (int i = 0; i < m_threadCount; ++i)
	{
		m_threadArry[i].join();
	}

	// 스레드가 종료된 후 리소스 반환
	CloseHandle(m_hEmptyCheckSemaphore);
	CloseHandle(m_hFullCheckSemaphore);
	CloseHandle(m_hPSemaphore);
	CloseHandle(m_hCSemaphore);

	delete[] m_jobQueue;
}


void ThreadPool::ThreadWork()
{
	while (true)
	{
		// consumer 스레드 임계구역 진입
		WaitForSingleObject(m_hCSemaphore, INFINITE);

		// job을 처리하기전 job이 있는지 확인
		// hFullCheckSemaphore이 양수일경우 job이 있으므로 진입한다
		// hFullCheckSemaphore이 0이면 job이 없으므로 들어올떄까지 대기
		WaitForSingleObject(m_hFullCheckSemaphore, INFINITE);

		std::function<void()> job = std::move(m_jobQueue[m_bufferOutIdx]);
		m_bufferOutIdx = (m_bufferOutIdx + 1) % m_maxJobCount;

		// job을 처리했으므로 다음 job을 받기위해 hEmptyCheckSemaphore 카운트 증가
		ReleaseSemaphore(m_hEmptyCheckSemaphore, 1, NULL);

		// consumer 스레드 임계구역 진입 종료
		ReleaseSemaphore(m_hCSemaphore, 1, NULL);

		// job 실행
		job();

		// 종료 처리
		if (m_bStopAll.load() == true && abs(m_bufferOutIdx - m_bufferInIdx) == 1)
		{
			break;
		}
	}
}

void ThreadPool::AddWork(std::function<void()> _delegateWork)
{
	// produce 스레드 임계구역 진입
	WaitForSingleObject(m_hPSemaphore, INFINITE);

	// job을 넣기전 hEmptyCheckSemaphore 세마포어 확인
	// hEmptyCheckSemaphore세마포어가 0이면 작업이 꽉 차있으므로 큐에 자리가 있을때까지 대기한다
	WaitForSingleObject(m_hEmptyCheckSemaphore, INFINITE);

	m_jobQueue[m_bufferInIdx] = std::move(_delegateWork);
	m_bufferInIdx = (m_bufferInIdx + 1) % m_maxJobCount;

	// job을 넣었으므로 full semaphore 카운트 증가
	ReleaseSemaphore(m_hFullCheckSemaphore, 1, NULL);

	// produce 스레드 임계구역 진입 종료
	ReleaseSemaphore(m_hPSemaphore, 1, NULL);
}

