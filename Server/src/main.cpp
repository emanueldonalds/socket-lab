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
#include "Transaction.h"
#include <sys/io.h>

#define PROTOPORT       5193 
#define QUEUESIZE       6    
#define BUF_SIZE 		2048

int     iVisits      =  0;   

void* client_handler(void *);

int main(int argc, char *argv[])
{
	struct  protoent *pProtocolTableEntry;
	struct  sockaddr_in sadServerAddress;
	struct  sockaddr_in sadClientAddress;

	int     iPortNumber;				
	int     iAddressLength;			
	int     sockedDesc, clientSocket;	

	memset((char *)&sadServerAddress,0,sizeof(sadServerAddress)); 
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
		fprintf(stderr,"bad port number %s\n",argv[1]);
		exit(1);
	}

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

	/*Create threads for each connection*/
	pthread_t thread_id;
	while( (clientSocket = accept(sockedDesc, (struct sockaddr *)&sadClientAddress, (socklen_t*)&iAddressLength)) )
	{
		puts("Connection established");

		if (pthread_create( &thread_id, NULL, client_handler, (void*) &clientSocket) < 0)
		{
			perror("Thread create failure");
			exit(1);
		}
	}
		pthread_join(thread_id, NULL);

        exit(0);
}

/* USE THIS
#include <iostream>
#include <sstream>
#include <map>
#include "Transaction.h"
#include "UserPosition.h"
#include <stdio.h>
*/

void * client_handler(void* socketDesc)
{
	int sock = *(int*)socketDesc;

/* USE THIS
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