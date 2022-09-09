#include <iostream>
#include "ThreadPool.h"


using namespace std;


void Test()
{
	cout << "test \n";
}

int main()
{
	ThreadPool pool(60);

	pool.AddWork([]() { Test(); });

	return 0;
}
