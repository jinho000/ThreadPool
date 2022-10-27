#include <iostream>
#include <Windows.h>
#include <queue>
#include <condition_variable>
#include <thread>
#include <mutex>
#include <functional>
#include <chrono>
#include <iomanip>
#include <stack>

#include "ThreadPool.h"
#include "MemoryPool.h"

using namespace std;
using namespace std::chrono;

constexpr long long ThreadOperationCount = 10000;
constexpr long long OperationCount = 10000 * ThreadOperationCount;

long long gNonAtomicValue = 0;
atomic<long long> gAtomicValue = 0;

void ThreadTest()
{
	long long value = 0;
	for (long long i = 0; i < ThreadOperationCount; ++i)
	{
		++value;
	}

	gAtomicValue.exchange(value);
}

void TestThreadPool()
{
	cout << "Test Multi Thread" << endl;

	auto start = high_resolution_clock::now();
	{
		ThreadPool threadPool(std::thread::hardware_concurrency(), 100);
		constexpr long long addWorkCount = OperationCount / ThreadOperationCount;
		for (long long i = 0; i < addWorkCount; ++i)
		{
			threadPool.AddWork([]() { ThreadTest(); });
		}
	}
	auto end = high_resolution_clock::now();

	cout << "Operation Result: " << gAtomicValue << endl;
	duration<double> checkTime = end - start;
	std::cout << "time: " << checkTime.count() << "s" << endl << endl;
}

struct TestMemPool
{
	int a;
	int b;
	int c;
};

constexpr int testCount = 8;
constexpr int countMem = 1024 * 1024 * testCount;
TestMemPool* intArray[countMem] = {};
TestMemPool* intArrayNew[countMem] = {};

void TestMemoryPool()
{
	cout << "Test Size: " << testCount * sizeof(TestMemPool) << "MB" << endl;

	MemoryPool pool(countMem * sizeof(TestMemPool), sizeof(TestMemPool));
	
	auto start = high_resolution_clock::now();
	
	for (int i = 0; i < countMem / 4; ++i)
	{
		intArray[i] = (TestMemPool*)pool.Allocate();
	}

	for (int i = 0; i < countMem / 4; ++i)
	{
		pool.DeAllocate(intArray[i]);
	}

	auto end = high_resolution_clock::now();

	duration<double> checkTime = end - start;
	std::cout << "time: " << checkTime.count() << "s" << endl << endl;


	start = high_resolution_clock::now();

	for (int i = 0; i < countMem / 4; ++i)
	{
		intArrayNew[i] = new TestMemPool;
	}

	for (int i = 0; i < countMem / 4; ++i)
	{
		delete intArrayNew[i];
	}

	end = high_resolution_clock::now();

	checkTime = end - start;
	std::cout << "time: " << checkTime.count() << "s" << endl << endl;
}

HANDLE hCSemaphore = CreateSemaphore(NULL, 0, 1, NULL);

void TestThread()
{
	int result = WaitForSingleObject(hCSemaphore, INFINITE);

	int a = 0;
}

int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	
	std::thread t1(TestThread);

	string input;
	cin >> input;

	CloseHandle(hCSemaphore);

	t1.join();

	return 0;
}
