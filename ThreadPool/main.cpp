#include <iostream>
#include "ThreadPool.h"

using namespace std;

void Func()
{
	
}

int main()
{
	ThreadPool pool(100);

	pool.AddWork([]() { Func(); });
	

	return 0;
}
