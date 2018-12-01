/*
This is a threaded server for updating stock market position data from clients
Usage: server [ port ]
*/

#ifndef __unix__
#include <winsock2.h>
#include <windows.h>
#else
#define closesocket close
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifndef __unix__
#include <io.h>
#else
#include <sys/io.h>
#endif

#define PROTOPORT 5193
#define QUEUESIZE 6

int iVisits = 0;

int main(int argc, char *argv[])
{
	struct protoent *pProtocolTableEntry;
	struct sockaddr_in sadServerAddress;
	struct sockaddr_in sadClientAddress;

	int iPortNumber;
	socklen_t iAddressLength;
	char szBuffer[1000];

#ifdef WIN32
	WSADATA wsaData;
	WORD wVersionRequested;
	int err;
	SOCKET sd, sd2;
	wVersionRequested = MAKEWORD(2, 2);
	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0)
	{
		return;
	}
#else
	int sd, sd2;
#endif

	memset((char *)&sadServerAddress, 0, sizeof(sadServerAddress));
	sadServerAddress.sin_family = AF_INET;
	sadServerAddress.sin_addr.s_addr = INADDR_ANY;

	if (argc > 1)
	{
		iPortNumber = atoi(argv[1]);
	}
	else
	{
		iPortNumber = PROTOPORT;
	}

	if (iPortNumber > 0)
		sadServerAddress.sin_port = htons((u_short)iPortNumber);
	else
	{
		fprintf(stderr, "bad port number %s\n", argv[1]);
		exit(1);
	}

	if ((pProtocolTableEntry = getprotobyname("tcp")) == 0)
	{
		fprintf(stderr, "cannot map \"tcp\" to protocol number");
		exit(1);
	}

	sd = socket(PF_INET, SOCK_STREAM, pProtocolTableEntry->p_proto);
	if (sd < 0)
	{
		fprintf(stderr, "socket creation failed.\n");
		exit(1);
	}

	if (bind(sd, (struct sockaddr *)&sadServerAddress, sizeof(sadServerAddress)) < 0)
	{
		fprintf(stderr, "bind failed.\n");
		exit(1);
	}

	if (listen(sd, QUEUESIZE) < 0)
	{
		fprintf(stderr, "listen failed.\n");
		exit(1);
	}

	while (iVisits < 5)
	{
		iAddressLength = sizeof(sadClientAddress);
		if ((sd2 = accept(sd, (struct sockaddr *)&sadClientAddress, &iAddressLength)) < 0)
		{
			fprintf(stderr, "accept failed.\n");
			exit(1);
		}
		iVisits++;
		sprintf(szBuffer, "This server has been contacted %d time%s\n", iVisits, iVisits == 1 ? "." : "s.");
		write(1, szBuffer, strlen(szBuffer));
		send(sd2, szBuffer, strlen(szBuffer), 0);
		closesocket(sd2);
	}

#ifdef WIN32
	WSACleanup();
#endif
	exit(0);
}

#include <iostream>
#include <sstream>
#include <map>
#include "Transaction.h"
#include "UserPosition.h"
#include <stdio.h>

void ClientHandler()
{
	/*
	std::map<std::string, UserPosition> users; //A structure for remembering users and thier positions

	for (int i = 0; i < 10; i++)
	{
		std::stringstream iss; //The incoming buffer stream
		iss << "Carl IBM s 24";

		Transaction *t = new Transaction(iss); //parsing the buffer to a transaction object

		//check if user is in map and retrieve or else add user to map
		UserPosition userPosition;
		std::map<std::string, UserPosition>::iterator it;
		it = users.find(t->GetUser());
		if (it != users.end())
		{
			userPosition = users[t->GetUser()];
		}
		else
		{
			userPosition = UserPosition(t->GetUser());
			users[t->GetUser()] = userPosition;
		}

		//do transaction
		userPosition.AddNewPosition(t->GetTicker(), t->GetAmount(), t->GetBS());
		//Add user again to save to the users structure
		users[t->GetUser()] = userPosition;

		//retransmit the updated position data to the client
		t->UpdateAmount(userPosition.FindPosition(t->GetTicker()));
		std::stringstream oss;
		t->OutputData(oss);

		delete t;
	}
	*/
}