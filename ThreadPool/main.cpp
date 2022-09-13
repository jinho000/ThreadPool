#include <iostream>
#include <Windows.h>
#include <queue>
#include <condition_variable>
#include <thread>
#include <mutex>
#include "ThreadPool.h"

using namespace std;

ThreadPool pool(10);

void ThreadWork()
{

	cout << "Thread Work " << GetCurrentThreadId() << endl;
	
}

int main()
{

	while (true)
	{
		pool.AddWork(ThreadWork);
	}

	return 0;
}
