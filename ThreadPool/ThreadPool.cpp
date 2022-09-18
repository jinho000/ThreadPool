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

	// �������� �ʱ�ȭ
	m_hEmptyCheckSemaphore = CreateSemaphore(NULL, m_maxJobCount, m_maxJobCount, NULL);
	m_hFullCheckSemaphore = CreateSemaphore(NULL, 0, m_maxJobCount, NULL);;

	m_hPSemaphore = CreateSemaphore(NULL, 1, 1, NULL);
	m_hCSemaphore = CreateSemaphore(NULL, 1, 1, NULL);

	// job ť ����
	m_jobQueue = new std::function<void()>[m_maxJobCount];

	// ������ ����
	for (int i = 0; i < m_threadCount; ++i)
	{
		m_threadArry.push_back(std::thread(&ThreadPool::ThreadWork, this));
	}
}

ThreadPool::~ThreadPool()
{
	// �����Ű������ job�� �־���
	m_bStopAll.store(true);

	for (int i = 0; i < m_threadCount; ++i)
	{
		AddWork([]() {});
	}

	for (int i = 0; i < m_threadCount; ++i)
	{
		m_threadArry[i].join();
	}

	// �����尡 ����� �� ���ҽ� ��ȯ
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
		// consumer ������ �Ӱ豸�� ����
		WaitForSingleObject(m_hCSemaphore, INFINITE);

		// job�� ó���ϱ��� job�� �ִ��� Ȯ��
		// hFullCheckSemaphore�� ����ϰ�� job�� �����Ƿ� �����Ѵ�
		// hFullCheckSemaphore�� 0�̸� job�� �����Ƿ� ���Ë����� ���
		WaitForSingleObject(m_hFullCheckSemaphore, INFINITE);

		std::function<void()> job = std::move(m_jobQueue[m_bufferOutIdx]);
		m_bufferOutIdx = (m_bufferOutIdx + 1) % m_maxJobCount;

		// job�� ó�������Ƿ� ���� job�� �ޱ����� hEmptyCheckSemaphore ī��Ʈ ����
		ReleaseSemaphore(m_hEmptyCheckSemaphore, 1, NULL);

		// consumer ������ �Ӱ豸�� ���� ����
		ReleaseSemaphore(m_hCSemaphore, 1, NULL);

		// job ����
		job();

		// ���� ó��
		if (m_bStopAll.load() == true && abs(m_bufferOutIdx - m_bufferInIdx) == 1)
		{
			break;
		}
	}
}

void ThreadPool::AddWork(std::function<void()> _delegateWork)
{
	// produce ������ �Ӱ豸�� ����
	WaitForSingleObject(m_hPSemaphore, INFINITE);

	// job�� �ֱ��� hEmptyCheckSemaphore �������� Ȯ��
	// hEmptyCheckSemaphore������� 0�̸� �۾��� �� �������Ƿ� ť�� �ڸ��� ���������� ����Ѵ�
	WaitForSingleObject(m_hEmptyCheckSemaphore, INFINITE);

	m_jobQueue[m_bufferInIdx] = std::move(_delegateWork);
	m_bufferInIdx = (m_bufferInIdx + 1) % m_maxJobCount;

	// job�� �־����Ƿ� full semaphore ī��Ʈ ����
	ReleaseSemaphore(m_hFullCheckSemaphore, 1, NULL);

	// produce ������ �Ӱ豸�� ���� ����
	ReleaseSemaphore(m_hPSemaphore, 1, NULL);
}

