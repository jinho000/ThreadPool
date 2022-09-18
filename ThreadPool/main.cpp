#include <iostream>
#include <Windows.h>
#include <queue>
#include <condition_variable>
#include <thread>
#include <mutex>
#include <functional>
#include <chrono>
#include <iomanip>

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

	gAtomicValue += value;
}


int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	
	{
		//ThreadPool pool(19, 10);
	}

	cout << "Operation Count: " << OperationCount << endl << endl;


	//////////////////////////////

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
	

	///////////////////////////////

	//cout << "Test Single Thread" << endl;

	//start = high_resolution_clock::now();
	//for (long long i = 0; i < OperationCount; ++i)
	//{
	//	++gNonAtomicValue;
	//}
	//end = high_resolution_clock::now();

	//cout << "Operation Result: " << gNonAtomicValue << endl;
	//checkTime = end - start;
	//std::cout << "time: " << checkTime.count() << "s" << endl;

	return 0;
}
