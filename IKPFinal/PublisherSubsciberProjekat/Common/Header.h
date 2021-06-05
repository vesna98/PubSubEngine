#pragma once

#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>


typedef struct publisherMessage {
	char topic[20];
	char message[40];
} PublisherMessage;

typedef struct subscriberMessage {
	int id;
	char topic[20];
} SubscriberMessage;

/*Used to receive data via TCP over network
	s:		socket used for communication
	buff:	data to be sent
	size:	length of buff

	returns number of bytes successfully received
*/
int Recv(SOCKET s, char* buff,int size);

/*Used to send data via TCP over network
	s:		socket used for communication
	buff:	data to be sent
	lenght: length of buff

	returns number of bytes successfully sent
*/
int Send(SOCKET s, char* buff, int lenght);

/*Used to conncet clients via TCP over network
	address:	client address
	port:		client port

	returns connected socket
*/
SOCKET Connect(char *adress,int port);

/*Used to initialize windows socket
	returns true if initialization is successful
*/
bool InitializeWindowsSockets();

/*Used to see in what state socket is
	s:		socket
	pom:	state

	returns number of sockets in that state or -1 if there's a socket error
*/
int Select(SOCKET s,int pom);


