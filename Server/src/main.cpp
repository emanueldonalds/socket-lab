/*
* Threaded server for keeping track of user positions
*/

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>

#include <stdio.h>
#include <string>
#include <sstream>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <sys/io.h>

#include <iostream>
#include <map>
#include "Transaction.h"
#include "UserPosition.h"

#define PROTOPORT       5193 
#define QUEUESIZE       6    
#define BUF_SIZE 		2048

pthread_mutex_t lock;

void* client_handler(void *);

struct clientHandlerArgs 
{
	//The arguments to be passed to each client handler thread
	int sClientSocket;
	std::map<std::string, UserPosition> *users; //A pointer to the user position structure that will be shared among threads.
};

int main(int argc, char *argv[])
{
	std::map<std::string, UserPosition> users; //A structure for remembering users and thier positions

	struct  protoent *pProtocolTableEntry;
	struct  sockaddr_in sadServerAddress;
	struct  sockaddr_in sadClientAddress;
	struct	clientHandlerArgs pClientHandlerArgs;

	int     iPortNumber;				
	int     iAddressLength;			
	int     sockedDesc;
	int 	clientSocket;

	memset((char *)&sadServerAddress,0,sizeof(sadServerAddress)); 
	sadServerAddress.sin_family = AF_INET;         
	sadServerAddress.sin_addr.s_addr = INADDR_ANY; 

	//INITIALIZE MUTEX
	if (pthread_mutex_init(&lock, NULL) != 0)
	{
		fprintf(stderr,"Mutex init failed\n");
		exit(1);
	}

	//CHECK IF USER HAS PROVIDED PORT NUMBER IN ARGS ELSE USE DEFAULT PORT
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
		fprintf(stderr,"bad port number %s\n",argv[1]);
		exit(1);
	}

	//SETUP SOCKET STUFF
	if ( (pProtocolTableEntry = getprotobyname("tcp")) == 0) 
	{
		fprintf(stderr, "cannot map \"tcp\" to protocol number");
		exit(1);
	}

	sockedDesc = socket(PF_INET, SOCK_STREAM, pProtocolTableEntry->p_proto);
	if (sockedDesc < 0)
	{
		fprintf(stderr, "socket creation failed.\n");
		exit(1);
	}

	int opt = 1;

	if(setsockopt(sockedDesc, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int)) < 0)
		perror("setsocketopt(SO_REUSEADDR) failed");

	if (bind(sockedDesc,(struct sockaddr *)&sadServerAddress, sizeof(sadServerAddress)) < 0)
	{
		fprintf(stderr, "bind failed.\n");
		exit(1);
	}

	if (listen(sockedDesc, QUEUESIZE) < 0)
	{
		fprintf(stderr, "listen failed.\n");
		exit(1);
	}
	
	puts("Waiting for connections...");

	//CREATE A THREAD FOR EACH CONNECTION
	pthread_t thread_id;
	while( (clientSocket = accept(sockedDesc, (struct sockaddr *)&sadClientAddress, (socklen_t*)&iAddressLength)) )
	{
		puts("NEW CONNECTION");

		//SETUP ARGUMENTS
		pClientHandlerArgs.sClientSocket = clientSocket;
		pClientHandlerArgs.users = &users;

		if (pthread_create( &thread_id, NULL, client_handler, (void*) &pClientHandlerArgs) < 0)
		{
			perror("Thread create failure");
			exit(1);
		}
	}
		pthread_join(thread_id, NULL);
		pthread_mutex_destroy(&lock);

        exit(0);
}

void * client_handler(void* args)
{
	struct clientHandlerArgs *pargs = (struct clientHandlerArgs *) args;
	int sock = pargs->sClientSocket;
	std::map<std::string, UserPosition> &users = {*pargs->users}; //All threads gets a pointer to the map structure
	int n;
	char serverBuffer[BUF_SIZE] = "CONNECTION ESTABLISHED";
	char transactionBuffer[BUF_SIZE];

	//SEND INITIAL BITS TO CLIENT
	n = send(sock, serverBuffer, strlen(serverBuffer) + 1, 0);
	if(n>0)
		printf("Sent: %d bits: %s\n", n, serverBuffer);
	else
		printf("Error sending initial bits, n: %d \n", n);

	//START LOOP
	while(true)
	{
		//RECIEVE TRANSACTION DETAILS (ALSO CHECK FOR QUIT MESSAGE)
		n=recv(sock, transactionBuffer, BUF_SIZE, 0);
		if(n > 0)
			printf("Recieved: %d bits: %s\n", n, transactionBuffer);
		else
		{
			printf("Error recieving, n: %d \n", n);
			break;
		}

		if (strncmp(transactionBuffer, "quit", 4) == 0)
		{
			printf("Quit message recieved\n");
			break;
		}

		//RECREATE THE TRANSACTION OBJECT
		std::stringstream iss;
		iss << transactionBuffer;

		Transaction *transaction = new Transaction(iss);
		
		//DO TRANSACTION
		pthread_mutex_lock(&lock); //Lock mutex while fiddling with the users map

		users[transaction->GetUser()].AddNewPosition(transaction->GetTicker(),
									 transaction->GetAmount(),
									 transaction->GetBS());

		//PREPARE BUFFER WITH THE UPDATE TICKER AMOUNT
		std::stringstream ssTickerAmount;
		ssTickerAmount << users[transaction->GetUser()].FindPosition(transaction->GetTicker());

		pthread_mutex_unlock(&lock); //The mutex can be unlocked here
		delete transaction;

		strcpy(serverBuffer, ssTickerAmount.str().c_str());


		//SEND BACK MESSAGE
		n = send(sock, serverBuffer, strlen(serverBuffer) + 1, 0);
		if(n>0)
			printf("Sent: %d bits: %s\n", n, serverBuffer);
		else
		{
			printf("Error sending initial bits, n: %d \n", n);
			break;
		}

	}//ITERATE LOOP

	//CLOSE CONNECTION
	printf("CLOSING CONNECTION\n");
	close(sock);
	return 0;
}