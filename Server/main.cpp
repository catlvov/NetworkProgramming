#include<iostream>
#include<WinSock2.h>
#include<WS2tcpip.h>
#include<iphlpapi.h>

#pragma comment(lib, "WS2_32.lib")

#define MTU 1500 

using std::cin;
using std::cout;
using std::endl;

void main()
{
	setlocale(LC_ALL, "");
	cout << "SERVER" << endl;

	INT iResult = 0;

	//0) инициализация WinSOCK
	WSADATA wsaData;
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

	//1 параметры подключения 
	addrinfo hints;
	addrinfo* target;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family		 = AF_INET;
	hints.ai_socktype    = SOCK_STREAM;
	hints.ai_protocol	 = IPPROTO_TCP;
	hints.ai_flags		 = AI_PASSIVE;

	iResult = getaddrinfo(NULL, "27015", &hints, &target);  // NULL - '0,0,0,0'. 
	if (iResult)
	{
		cout << "getaddrinfo() failed with error:" << iResult << endl;
		cout << "функция getaddrinfo() завершилась с ошибкой:" << iResult << endl;
		WSACleanup();
		return;
	}

	//2
	SOCKET listen_socket = socket(target->ai_family, target->ai_socktype, target->ai_protocol);
	if (listen_socket == INVALID_SOCKET)
	{
		cout << "LISTEM SOKET() creation failed with error: " << WSAGetLastError() << endl;
		cout << "функция LISTEM SOKET() завершилась с ошибкой:" << WSAGetLastError() << endl;
		freeaddrinfo(target);
		WSACleanup();
		return;
	}

	//3) BIND - привязываем серверный сокет к интерфейсу который он будет слушать 
	iResult = bind(listen_socket, target->ai_addr, target->ai_addrlen);
	if (iResult)
	{
		cout << "BIND failed with error" << WSAGetLastError() << endl;
		cout << "Привязка сокета к порту завершилась с ошибкой" << WSAGetLastError() << endl;
		closesocket(listen_socket);
		freeaddrinfo(target);
		WSACleanup();
		return;
	}

	//4) -  запускаем прослушавание порта 
	if (listen(listen_socket, 1) == SOCKET_ERROR)
	{
		cout << "Listen faiked with error: " << WSAGetLastError() << endl;
		cout << "Прослушивать порт невозможно иза ошибки: " << WSAGetLastError() << endl;
		closesocket(listen_socket);
		freeaddrinfo(target);
		WSACleanup();
		return;
	}

	//5 принимаем подклбчение от клиентов 
	SOCKET client_soket = accept(listen_socket, NULL, NULL);
	if (client_soket == INVALID_SOCKET)
	{
		cout << "Accept failed with error: " << WSAGetLastError() << endl;
		cout << "Не удалось принять подключение от клиента: " << WSAGetLastError() << endl;
		closesocket(listen_socket);
		freeaddrinfo(target);
		WSACleanup();
		return;
	}
	//6) получение и отправка данных от клиента 
	CHAR recv_buffer[MTU] = {};
	iResult = recv(client_soket,recv_buffer, MTU, NULL);
	if (iResult > 0)
	{
		cout << iResult << "Bytes received. message: " << recv_buffer << endl;

	}
	else if (iResult == 0) cout << "Nothinf received, connection cloin \n нет данных от клиента" << endl;
	else
	{
		cout << "Receive failed with error: " << WSAGetLastError() << endl;
		cout << "При получении данных возникла: " << WSAGetLastError() << endl;
	}

	//7) отправка данных клиенту 
	cin.get();
	CHAR send_buffer[MTU] = "Привет Клиент, Ваше сообщение: ";
	sprintf(send_buffer, "Привет Клиент, Ваше сообщение: %s" , recv_buffer);
	iResult = send(client_soket, send_buffer, strlen(send_buffer), NULL);
	if (iResult == SOCKET_ERROR) 
		cout << "send() faiked with error; ";
	//8) 
	iResult = shutdown(client_soket, SD_BOTH);
	if (iResult)
	{
		cout << "shutdown failed with error: " << WSAGetLastError() << endl;
		cout << "приотправке данных возникла ошибка " << WSAGetLastError() << endl;
	}
	//9) освоболить русурсы
	closesocket(client_soket);
	closesocket(listen_socket);
	freeaddrinfo(target);
	WSACleanup();
}