#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iphlpapi.h>
#include <cstring>   
#include <locale>    

#pragma comment(lib, "WS2_32.lib")

using std::cin;
using std::cout;
using std::endl;

#define MTU		1500		

void main()
{
	setlocale(LC_ALL, "");

	INT iResult;	

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
	if (connect_socket == INVALID_SOCKET)
	{
		cout << "Socket creation failed with error " << WSAGetLastError() << endl;
		freeaddrinfo(target);
		WSACleanup();
		return;
	}

	//3) Подключаемся к Серверу:
	iResult = connect(connect_socket, target->ai_addr, target->ai_addrlen);
	if (iResult == SOCKET_ERROR)
	{
		closesocket(connect_socket);
		connect_socket = INVALID_SOCKET;
	}
	freeaddrinfo(target);
	if (connect_socket == INVALID_SOCKET)
	{
		cout << "Unable to connect to Server. Error: " << WSAGetLastError() << endl;
		WSACleanup();
		return;
	}

	//4) Отправка данных на Сервер:
	CHAR send_buffer[MTU] = "Привет Сервер!";
	iResult = send(connect_socket, send_buffer, strlen(send_buffer), 0);
	if (iResult == SOCKET_ERROR)
	{
		cout << "Send failed with error: " << WSAGetLastError() << endl;
		closesocket(connect_socket);
		WSACleanup();
		return;
	}
	cout << "Sent " << iResult << " Bytes" << endl;

	//5) Получение данных от Сервера:
	CHAR recv_buffer[MTU] = {};
	iResult = recv(connect_socket, recv_buffer, MTU, NULL);
	if (iResult > 0)cout << iResult << "Byte received. Message: " << recv_buffer << endl;
	else if (iResult == 0)cout << "Nothing received." << endl;
	else cout << "Receive failed with error: " << WSAGetLastError() << endl;

	//6) Завершаем сеанс работы с Сервером и освобождаем ресурсы:
	iResult = shutdown(connect_socket, SD_BOTH);
	if (iResult == SOCKET_ERROR)cout << "Shutdown failed with error: " << WSAGetLastError() << endl;
	closesocket(connect_socket);
	WSACleanup();
}