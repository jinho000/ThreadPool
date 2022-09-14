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

class Test
{
public:
	int a;
	double d;
	char b;

	virtual void func() {}
};

constexpr int MB = 1024 * 1024;
constexpr int additional = 32;
constexpr int Count = MB * additional;
MemoryPool pool(Count * sizeof(Test), sizeof(Test));
Test* pTestPoolArry[Count] = {};
Test* pTestNewArry[Count] = {};

int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	
	cout << "Test Object Size: " << sizeof(Test) << endl;
	cout << "Test Object Count: " << Count << endl;
	cout << "Test Memory Size: " << additional * sizeof(Test) << "MB" << endl << endl;


	///////////////////////

	cout << "Test Pool Allocate" << endl;
	auto start = high_resolution_clock::now();

	for (int i = 0; i < Count; ++i)
	{
		pTestPoolArry[i] = reinterpret_cast<Test*>(pool.Allocate());
		pTestPoolArry[i]->a = i;
		pTestPoolArry[i]->b = 'b';
		pTestPoolArry[i]->d = 3.2;
	}

	for (int i = 0; i < Count; ++i)
	{
		pool.DeAllocate(pTestPoolArry[i]);
		pTestPoolArry[i] = NULL;
	}
	
	auto end = high_resolution_clock::now();
	duration<double> checkTime = end - start;
	std::cout << "time: " << checkTime.count() << "s" << endl << endl;
	

	////////////////////

	cout << "Test New Allocate " << endl;
	start = high_resolution_clock::now();
	for (int i = 0; i < Count; ++i)
	{
		pTestNewArry[i] = new Test;
		pTestNewArry[i]->a = i;
		pTestNewArry[i]->b = 'b';
		pTestNewArry[i]->d = 3.2;
	}

	for (int i = 0; i < Count; ++i)
	{
		delete pTestNewArry[i];
		pTestNewArry[i] = NULL;
	}

	end = high_resolution_clock::now();
	checkTime = end - start;
	std::cout << "time: " << checkTime.count() << "s" << endl;

	return 0;
}
