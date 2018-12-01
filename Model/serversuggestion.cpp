#include <iostream>
#include <conio.h>
#include <sstream>
#include <map>
#include "Transaction.h"
#include "UserPosition.h"

int main()
{
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
	

	_getch();
	return 0;
}