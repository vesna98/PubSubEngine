#define WIN32_LEAN_AND_MEAN
#pragma comment (lib, "Ws2_32.lib")

#define _WINSOCK_DEPRECATED_NO_WARNINGS 1
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include "../Common/Header.h"
#include <conio.h>
#include <string.h>

#define DEFAULT_BUFLEN 250
#define DEFAULT_PORT 5002
#define ADDRESS "127.0.0.1"


void Subscribe(SOCKET connectSocket, SubscriberMessage* subMessage);

int __cdecl main(int argc, char** argv)
{
	SubscriberMessage subMessage;
	char temp[20];

	printf("Unesite svoj ID:");
	gets_s(temp, 20);
	subMessage.id = atoi(temp);

	SOCKET connectSocket = Connect((char*)ADDRESS, DEFAULT_PORT);
	if (connectSocket == INVALID_SOCKET)
	{
		printf("Greska pri konektovanju.\n");
		return -1;
	}

	unsigned long int nonBlockingMode = 1;
	ioctlsocket(connectSocket, FIONBIO, &nonBlockingMode);

	fd_set readfds;
	timeval timeVal;
	timeVal.tv_sec = 1;
	timeVal.tv_usec = 0;
	
	while (true)
	{
		FD_ZERO(&readfds);
		FD_SET(connectSocket, &readfds);

		int result = select(0, &readfds, NULL, NULL, &timeVal);

		if (result == 0) {
			if (_kbhit()) {
				printf("Unesite temu na koju zelite da se pretplatite (ili 'EXIT'):");
				gets_s(subMessage.topic, 20);

				if (strcmp(subMessage.topic, "EXIT") == 0) {
					break;
				}

				Subscribe(connectSocket, &subMessage);
			}
		}
		else if (result == SOCKET_ERROR) {
			printf("Greska pri pozivu select-a.");
			break;
		}
		else {
			if (FD_ISSET(connectSocket, &readfds)) {
				PublisherMessage pubMessage;
				int iResult = Recv(connectSocket, (char*)&pubMessage, sizeof(PublisherMessage));

				printf("[%s]: %s\n", pubMessage.topic, pubMessage.message);
			}
		}
	}

	shutdown(connectSocket, JL_BOTH);
	closesocket(connectSocket);

	return 0;
}


void Subscribe(SOCKET connectSocket, SubscriberMessage* subMessage)
{
	int iResult;
	int i;

	iResult = Send(connectSocket, (char*)subMessage, sizeof(SubscriberMessage));

	if (iResult == SOCKET_ERROR)
	{
		printf("send failed with error: %d\n", WSAGetLastError());
		closesocket(connectSocket);
		WSACleanup();
		return;
	}

	printf("Bytes Sent: %ld\n", iResult);
}

