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
#define DEFAULT_PORT 5001
#define ADDRESS "127.0.0.1"


void Publish(SOCKET connectSocket, PublisherMessage* pubMessage);

int __cdecl main(int argc, char** argv)
{
	PublisherMessage pubMessage;

	SOCKET connectSocket = Connect((char*)ADDRESS, DEFAULT_PORT);
	if (connectSocket == INVALID_SOCKET)
	{
		printf("Greska pri konektovanju.\n");
		return -1;
	}

	unsigned long int nonBlockingMode = 1;
	ioctlsocket(connectSocket, FIONBIO, &nonBlockingMode);

	while (true)
	{
		printf("Unesite temu (ili 'EXIT'):");
		gets_s(pubMessage.topic, 20);

		if (strcmp(pubMessage.topic, "EXIT") == 0) {
			break;
		}

		printf("Unesite vasu vest:");
		gets_s(pubMessage.message, 40);

		Publish(connectSocket, &pubMessage);
	}

	shutdown(connectSocket, JL_BOTH);
	closesocket(connectSocket);

	return 0;
}


void Publish(SOCKET connectSocket, PublisherMessage* pubMessage)
{
	int iResult;
	int i;

	iResult = Send(connectSocket, (char*)pubMessage, sizeof(PublisherMessage));

	if (iResult == SOCKET_ERROR)
	{
		printf("send failed with error: %d\n", WSAGetLastError());
		closesocket(connectSocket);
		WSACleanup();
		return;
	}

	printf("Bytes Sent: %ld\n", iResult);
}
