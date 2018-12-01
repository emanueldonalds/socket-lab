/*
Client program for sending stock market position transactions to a server
Usage: Client $(IPADDR) $(PORT)
*/

#include <winsock.h>
#include <windows.h>
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <sstream>
#include <streambuf>
#include <stdlib.h>
#include <conio.h>
#include <string>
#include "Transaction.h"
#include <io.h>

#pragma comment(lib, "Ws2_32.lib")

#define PROTOPORT       5193
#define	BUF_SIZE		2048

char    szLocalHost[] = "localHost";   
										
int main(int argc, char* argv[])
{
	struct  hostent  *pHostTableEntry;		
	struct  protoent *pProtocolTableEntry;	
	struct  sockaddr_in sadServerAddress;	
	int     iPortNumber;					
	char    *szHost;						
	int     n;								
	char    szBuffer[BUF_SIZE];				
	WSADATA wsaData;
	WORD wVersionRequested;
	int err;
	SOCKET     sd;								
	wVersionRequested = MAKEWORD(2, 2);
	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0)
	{
		return 1;
	}
	
	memset((char *)&sadServerAddress, 0, sizeof(sadServerAddress)); 
	sadServerAddress.sin_family = AF_INET;        

	if (argc > 2)
	{
		iPortNumber = atoi(argv[2]);  
	}
	else
	{
		iPortNumber = PROTOPORT;       
	}

	if (iPortNumber > 0)
		sadServerAddress.sin_port = htons((u_short)iPortNumber);
	else
	{
		fprintf(stderr, "bad port number %s\n", argv[2]);
		exit(1);
	}

	if (argc > 1)
	{
		szHost = argv[1];
	}
	else
	{
		szHost = szLocalHost;
	}

	pHostTableEntry = gethostbyname(szHost);

	if (pHostTableEntry == 0)
	{
		fprintf(stderr, "invalid szHost: %s\n", szHost);
		exit(1);
	}

	memcpy(&sadServerAddress.sin_addr, pHostTableEntry->h_addr, pHostTableEntry->h_length);

	if ((pProtocolTableEntry = getprotobyname("tcp")) == 0)
	{
		fprintf(stderr, "cannot map \"tcp\" to protocol number");
		exit(1);
	}

	sd = socket(PF_INET, SOCK_STREAM, pProtocolTableEntry->p_proto);
	if (sd < 0)
	{
		fprintf(stderr, "socket creation failed\n");
		exit(1);
	}

	if (connect(sd, (struct sockaddr *)&sadServerAddress, sizeof(sadServerAddress)) < 0)
	{
		fprintf(stderr, "socket creation failed.\n");
		exit(1);
	}

	printf("Client got connected to the server.\n");

	//CREATE A NEW TRANSACTION
	std::stringstream transactionDetails;
	transactionDetails << "Carl IBM b 120";
	Transaction transaction(transactionDetails);

	//CONVERT TRANSACTION DATA TO BUFFER DATA
	char tBuffer[BUF_SIZE];
	std::stringstream ostr;
	transaction.OutputData(ostr);
	strcpy_s(tBuffer, ostr.str().c_str());
	printf("Prepared the buffer with: %s\n", tBuffer);
	
	//
	
	n = recv(sd, szBuffer, BUF_SIZE, 0);
	printf("Recieved: %d bits: %s\n", n, szBuffer);
	
	//SEND BUFFER TO SERVER
	n =  send(sd, tBuffer, strlen(tBuffer)+1, 0);
	printf("Sent: %d bits: %s\n", n, tBuffer);

	closesocket(sd);
	_getch();

#ifdef WIN32
	WSACleanup();
#endif

	/* Terminate the client program gracefully. */
	exit(0);
}


