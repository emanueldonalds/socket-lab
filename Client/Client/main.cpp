/*
* Client program for sending stock market position transactions to a server
* Usage: Client $(IPADDR) $(PORT)
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
#include <string.h>
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
	int		quit;
	char    serverBuffer[BUF_SIZE];
	char	transactionBuffer[BUF_SIZE];
	WSADATA wsaData;
	WORD	wVersionRequested;
	int		err;
	SOCKET  sd;		
	std::string user;

	wVersionRequested = MAKEWORD(2, 2);
	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0)
	{
		return 1;
	}
	
	memset((char *)&sadServerAddress, 0, sizeof(sadServerAddress)); 
	sadServerAddress.sin_family = AF_INET;        

	//CHECK PARAMETERS AND SET DEFAULT VALUES
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

	//PROMT USER NAME
	std::cout << "User name: ";
	std::cin >> user;

	//CONNECT TO SERVER
	pHostTableEntry = gethostbyname(szHost);

	if (pHostTableEntry == 0)
	{
		fprintf(stderr, "invalid szHost: %s\n", szHost);
		_getch();
		exit(1);
	}

	memcpy(&sadServerAddress.sin_addr, pHostTableEntry->h_addr, pHostTableEntry->h_length);

	if ((pProtocolTableEntry = getprotobyname("tcp")) == 0)
	{
		fprintf(stderr, "cannot map \"tcp\" to protocol number");
		_getch();
		exit(1);
	}

	sd = socket(PF_INET, SOCK_STREAM, pProtocolTableEntry->p_proto);
	if (sd < 0)
	{
		fprintf(stderr, "socket creation failed\n");
		_getch();
		exit(1);
	}

	if (connect(sd, (struct sockaddr *)&sadServerAddress, sizeof(sadServerAddress)) < 0)
	{
		fprintf(stderr, "socket creation failed.\n");
		_getch();
		exit(1);
	}

	printf("Client got connected to the server.\n");

	//RECIEVE SOME BITS FROM SERVER
	n = recv(sd, serverBuffer, BUF_SIZE, 0);
	if (n <= 0)
	{
		printf("Error recieving buffer");
		_getch();
		exit(1);
	}
	printf("Server message: %s\n", serverBuffer);

	//START LOOP
	while (true)
	{
		//PROMPT USER FOR TRASACTION DETAILS
		std::string ticket;
		std::string bs;
		int amount;

		std::cout << "(1)\tBuy\n(2)\tSell\n";
		std::cin >> bs;
		std::cout << std::endl;
		if (bs == "1")
		{
			bs = "b";
			std::cout << "BUYING STOCK\n\n";
		}
		else
		{
			bs = "s";
			std::cout << "SELLING STOCK\n\n";
		}
		std::cout << "Stock ticket:\t";
		std::cin >> ticket;
		std::cout << "Ticket amount:\t";
		while (!(std::cin >> amount))
		{
			std::cout << "Enter an integer number\t";
			std::cin.clear();
			std::cin.ignore(INT_MAX, '\n');
		}
		std::cout << std::endl;
		//_getch();

		//CREATE A NEW TRANSACTION
		std::stringstream transactionDetails;
		transactionDetails << user << " " << ticket << " " << bs << " " << amount;
		Transaction transaction(transactionDetails);

		//CONVERT TRANSACTION DATA TO BUFFER DATA
		std::stringstream ostr;
		transaction.OutputData(ostr);
		strcpy_s(transactionBuffer, ostr.str().c_str());

		//SEND BUFFER TO SERVER
		n = send(sd, transactionBuffer, strlen(transactionBuffer) + 1, 0);

		//RECIEVE UPDATE MESSAGE FROM SERVER
		n = recv(sd, serverBuffer, BUF_SIZE, 0);
		if (n <= 0)
		{
			printf("Error recieving buffer");
			_getch();
			exit(1);
		}

		//DISPLAY MESSAGE
		printf("Recieved updated ticker amount for \"%s\": %s\n", transaction.GetTicker().c_str(), serverBuffer);

		//CHECK IF QUIT OR CONTINUE
		printf("\n(1)\tDo another transaction\n(2)\tQuit\n");
		std::cin >> quit;
		if (quit == 2)
		{
			n = send(sd, "quit", 4, 0);
			printf("Exiting\n");
			break;
		}

	}//ITERATE LOOP

	closesocket(sd);
	_getch();

#ifdef WIN32
	WSACleanup();
#endif

	/* Terminate the client program gracefully. */
	exit(0);
}


