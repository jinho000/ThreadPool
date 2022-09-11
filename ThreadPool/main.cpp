#include <iostream>
#include <Windows.h>

using namespace std;

constexpr int ThreadCount = 20;

HANDLE semaphore = NULL;
HANDLE threadArry[ThreadCount] = {};

DWORD WINAPI ThreadProc(LPVOID lpParam)
{
    // lpParam not used in this example
    UNREFERENCED_PARAMETER(lpParam);

    DWORD dwWaitResult;
    BOOL bContinue = TRUE;

    while (bContinue)
    {
        // Try to enter the semaphore gate.

        dwWaitResult = WaitForSingleObject(
            semaphore,      // handle to semaphore
            INFINITE);            // zero-second time-out interval

        switch (dwWaitResult)
        {
            // The semaphore object was signaled.
        case WAIT_OBJECT_0:
            // TODO: Perform task
            printf("Thread %d: wait succeeded\n", GetCurrentThreadId());
            bContinue = FALSE;

            // Simulate thread spending time on task
            Sleep(5);

            // Release the semaphore when task is finished
            if (!ReleaseSemaphore(
                semaphore,      // handle to semaphore
                1,              // increase count by one
                NULL))          // not interested in previous count
            {
                printf("ReleaseSemaphore error: %d\n", GetLastError());
            }
            break;

            // The semaphore was nonsignaled, so a time-out occurred.
        case WAIT_TIMEOUT:
            printf("Thread %d: wait timed out\n", GetCurrentThreadId());
            break;
        }
    }
    return TRUE;
}

int main()
{
	semaphore = CreateSemaphore(NULL, 10, 10, NULL);

    for (int i = 0; i < ThreadCount; i++)
    {
        threadArry[i] = CreateThread(
            NULL,       // default security attributes
            0,          // default stack size
            (LPTHREAD_START_ROUTINE)ThreadProc,
            NULL,       // no thread function arguments
            0,          // default creation flags
            NULL); // receive thread identifier

        if (threadArry[i] == NULL)
        {
            printf("CreateThread error: %d\n", GetLastError());
            return 1;
        }
    }

	WaitForMultipleObjects(ThreadCount, threadArry, TRUE, INFINITE);

	for (int i = 0; i < ThreadCount; i++)
		CloseHandle(threadArry[i]);

	CloseHandle(semaphore);

	return 0;
}
