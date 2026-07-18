#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iphlpapi.h>
#include <FormatLastError.h>

#pragma comment(lib, "WS2_32.lib")
#pragma comment(lib, "FormatLastError.lib")

using std::cin;
using std::cout;
using std::endl;

#define MTU		1500	

//CHAR* FormatLastError(DWORD dwError, CHAR szError[]);

void main()
{
	setlocale(LC_ALL, "");

	INT iResult;	
	DWORD dwError;
	CHAR szError[256]; 

	//0) Инициализация WinSOCK:
	WSADATA wsaData;
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult)
	{
		cout << "WSAStartup failed with error: " << iResult << endl;
		return;
	}

	//1) Задаем параметры подключения:
	addrinfo hints;
	addrinfo* target;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;			
	hints.ai_socktype = SOCK_STREAM;	
	hints.ai_protocol = IPPROTO_TCP;

	iResult = getaddrinfo("127.0.0.1", "27015", &hints, &target);	
	if (iResult)
	{
		cout << "getaddrinfo() failed with error " << iResult << endl;
		WSACleanup();
		return;
	}

	//2) Создаем сокет, при помощи которого будем подлючаться к Серверу:
	SOCKET connect_socket = socket(target->ai_family, target->ai_socktype, target->ai_protocol);
	dwError = WSAGetLastError();
	if (connect_socket == INVALID_SOCKET)
	{
		cout << FormatLastError(dwError, szError) << endl;
		cout << "Socket creation failed with error " << WSAGetLastError() << endl;
		freeaddrinfo(target);
		WSACleanup();
		return;
	}

	//3) Подключаемся к Серверу:
	iResult = connect(connect_socket, target->ai_addr, target->ai_addrlen);
	if (iResult)
	{
		//DWORD dwError = WSAGetLastError();
		/*LPSTR lpError = NULL;
		FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			dwError,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL),
			(LPSTR)&lpError,
			256,
			NULL
		);*/
		cout << FormatLastError(dwError, szError) << endl;
		cout << "Unable to connect to Server. Error: " << WSAGetLastError() << endl;
		closesocket(connect_socket);
		freeaddrinfo(target);
		WSACleanup();
		return;
	}

	//4) Отправка данных на Сервер:
	CHAR send_buffer[MTU] = "Привет Сервер!";
	iResult = send(connect_socket, send_buffer, strlen(send_buffer), 0);
	if (iResult == SOCKET_ERROR)
	{
		cout << "Send failed with error: " << WSAGetLastError() << endl;
		cout << FormatLastError(dwError, szError) << endl;
		closesocket(connect_socket);
		WSACleanup();
		return;
	}
	cout << "Sent " << iResult << " Bytes" << endl;

	//5) Получение данных от Сервера:
	CHAR recv_buffer[MTU] = {};
	iResult = recv(connect_socket, recv_buffer, MTU, NULL);
	dwError = WSAGetLastError();
	if (iResult > 0)cout << iResult << "Byte received. Message: " << recv_buffer << endl;
	else if (iResult == 0)cout << "Nothing received." << endl;
	else cout << "Receive failed with error: " << WSAGetLastError() << endl << FormatLastError(dwError,szError) << endl;

	//6) Завершаем сеанс работы с Сервером и освобождаем ресурсы:
	iResult = shutdown(connect_socket, SD_BOTH);
	if (iResult == SOCKET_ERROR)cout << "Shutdown failed with error: " << WSAGetLastError() << endl << FormatLastError(dwError, szError);
	closesocket(connect_socket);
	WSACleanup();
}
//
//CHAR* FormatLastError(DWORD dwError, CHAR szError[])
//{
//	ZeroMemory(szError, strlen(szError));
//	LPSTR lpError = NULL;
//	FormatMessage(
//		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
//		NULL,
//		dwError,
//		MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL),
//		(LPSTR)&lpError,
//		256,
//		NULL
//	);
//	//strcpy(szError, lpError);
//	sprintf(szError, "Error #%i: %s", dwError, lpError);
//	return szError;
//}