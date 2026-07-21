#include <Windows.h>
#include <iostream>

using std::cin;
using std::cout;
using std::endl;

void plus()
{
	while (true)
	{
		cout << "+ ";
	}
}

void minus()
{
	while (true)
	{
		cout << "- ";
	}
}

void main()
{
	setlocale(LC_ALL, "");
	/*plus();
	minus();*/
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
	WaitForMultipleObjects(2, hThreads, TRUE, INFINITE);
	CloseHandle(hThreads[0]);
	CloseHandle(hThreads[1]);

}