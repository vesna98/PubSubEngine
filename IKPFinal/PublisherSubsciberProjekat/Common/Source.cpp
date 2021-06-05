#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "Header.h"
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>


#define DEFAULT_BUFLEN 250
#define DEFAULT_PORT 27016
#define SERVER_SLEEP_TIME 50

int Recv(SOCKET s, char*buff,int size)
{
	int count = 0;
	int pom = 0;
	
	while (count < size)
	{
		int i = Select(s, 1);
		pom = recv(s, buff + count, size - count, 0);
		if (pom > 0)
		{
			count += pom;
		}
	}
		
	return count;
}

int Send(SOCKET s, char* buff, int lenght) {

	int count = 0;
	
	while (count != lenght) {
		int iResult = Select(s, 2);
		if (iResult > 0)
		{
			count += send(s, buff + count, lenght - count, 0);
		}
	}

	return count;
}


SOCKET Connect( char *adress, int port) {
	// socket used to communicate with server
	SOCKET connectSocket = INVALID_SOCKET;
	// variable used to store function return value
	//char *messageToSend = (char*)calloc(1, DEFAULT_BUFLEN);


	if (InitializeWindowsSockets() == false)
	{
		// we won't log anything since it will be logged
		// by InitializeWindowsSockets() function
		return INVALID_SOCKET;
	}

	// create a socket
	connectSocket = socket(AF_INET,
		SOCK_STREAM,
		IPPROTO_TCP);

	if (connectSocket == INVALID_SOCKET)
	{
		printf("socket failed with error: %ld\n", WSAGetLastError());
		WSACleanup();
		return INVALID_SOCKET;
	}

	// create and initialize address structure
	sockaddr_in serverAddress;
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = inet_addr(adress);
	serverAddress.sin_port = htons(port);
	// connect to server specified in serverAddress and socket connectSocket
	if (connect(connectSocket, (SOCKADDR*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR)
	{
		printf("Unable to connect to server.\n");
		closesocket(connectSocket);
		WSACleanup();
		return INVALID_SOCKET;
	}	

	return connectSocket;
}

bool InitializeWindowsSockets()
{
	WSADATA wsaData;
	// Initialize windows sockets library for this process
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		printf("WSAStartup failed with error: %d\n", WSAGetLastError());
		return false;
	}
	return true;
}


int Select(SOCKET s,int pom)
{
	int iResult;
	do {
		FD_SET set;
		timeval timeVal;


		FD_ZERO(&set);
		// Add socket we will wait to read from
		FD_SET(s, &set);

		// Set timeouts to zero since we want select to return
		// instantaneously
		timeVal.tv_sec = 0;
		timeVal.tv_usec = 0;

		if (pom == 1)
		{
			iResult = select(0 /* ignored */, &set, NULL, NULL, &timeVal);
		}

		if (pom == 2)
		{
			iResult = select(0 /* ignored */, NULL, &set, NULL, &timeVal);
		}

		if (iResult == SOCKET_ERROR)
		{
			printf("Greska!\n");
			return -1;
		}

		if (iResult == 0)
		{
			Sleep(SERVER_SLEEP_TIME);
			continue;
		}
		if (iResult > 0)
		{
			return iResult;
		}

	} while (1);

	
	
}



