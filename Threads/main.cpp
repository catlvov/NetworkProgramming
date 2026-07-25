#include <Windows.h>
#include <iostream>

using std::cin;
using std::cout;
using std::endl;

BOOL finish = FALSE;
HANDLE g_hMutex = NULL;

void plus()
{
	while (!finish)
	{
		WaitForSingleObject(g_hMutex, INFINITE);
		cout << "+ ";
		Sleep(100);
		ReleaseMutex(g_hMutex);
	}
}

void minus()
{
	while (!finish)
	{
		WaitForSingleObject(g_hMutex, INFINITE);
		cout << "- ";
		Sleep(100);
		ReleaseMutex(g_hMutex);
	}
}

void main()
{
	setlocale(LC_ALL, "");
	/*plus();
	minus();*/
	g_hMutex = CreateMutex(NULL, NULL, "Mutex");
	HANDLE hThreads[2] = {};
	hThreads[0] = CreateThread
	(
		NULL,
		0,
		(LPTHREAD_START_ROUTINE)plus,
		NULL,
		0,
		NULL
	);
	hThreads[1] = CreateThread
	(
		NULL,
		0,
		(LPTHREAD_START_ROUTINE)minus,
		NULL,
		0,
		NULL
	);
	cin.get();
	finish = TRUE;
	WaitForMultipleObjects(2, hThreads, TRUE, INFINITE);
	CloseHandle(hThreads[0]);
	CloseHandle(hThreads[1]);
	CloseHandle(g_hMutex);
}