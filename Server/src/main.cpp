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

std::map<std::string, UserPosition> users; //A structure for remembering users and thier positions

void* client_handler(void *);

struct clientHandlerArgs 
{
	int sClientSocket;
	int pipeFd[2];
};

int main(int argc, char *argv[])
{
	//TODO: decide between pipe or mutex and create something that updates the structure
	/*
	UserPosition userPosition("Carl");
	userPosition.AddNewPosition("IBM", 120, 'b');
	userPosition.AddNewPosition("IBM", 180, 'b');
	userPosition.AddNewPosition("APP", 20, 'b');
	userPosition.AddNewPosition("APP", 10, 's');
	userPosition.AddNewPosition("BNP", 20, 's');
	userPosition.AddNewPosition("BNP", 10, 's');
	//Add user again to save to the users structure
	users["Carl"] = userPosition;
	*/
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

	//CHECK FOR PROVIDED PORT NUMBER IN ARGS
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
		pClientHandlerArgs.sClientSocket = clientSocket;
		
		//CREATE A PIPE FOR EACH CONNECTION
		if (pipe(pClientHandlerArgs.pipeFd) == -1)
		{
			perror("Pipe create failure");
			exit(1);
		}

		if (pthread_create( &thread_id, NULL, client_handler, (void*) &pClientHandlerArgs) < 0)
		{
			perror("Thread create failure");
			exit(1);
		}
	}
		pthread_join(thread_id, NULL);

        exit(0);
}

void * client_handler(void* args)
{
	struct clientHandlerArgs *pargs = (struct clientHandlerArgs *) args;
	int sock = pargs->sClientSocket;
	int pipeFd[2] = {pargs->pipeFd[0], pargs->pipeFd[1]};
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

		//DO TRANSACTION (GENERATE RETURN MESSAGE)
		strcpy(serverBuffer, "TRANSACTION RETURN MESSAGE");

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

/*	std::map<std::string, UserPosition> users; //A structure for remembering users and thier positions

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
	/* dont use this
	int sock = *(int*)socketDesc;
	int n;
	char message[BUF_SIZE] = "Hello from client handler";
	char tBuffer[BUF_SIZE];

	
	
	n = send(sock, message, strlen(message)+1, 0);
	if(n>0)
		printf("Sent: %d bits: %s\n", n, message);
	else
		printf("Error sending, n: %d \n", n);

	n=recv(sock, tBuffer, BUF_SIZE, 0);
	if(n > 0)
		printf("Recieved: %d bits: %s\n", n, tBuffer);
	else
		printf("Error recieving, n: %d \n", n);

	std::stringstream istr;
	Transaction t;
	istr << tBuffer;
	istr >> t;
	std::cout << t << std::endl;

*/
	printf("CLOSING CONNECTION\n");
	close(sock);
	return 0;
}