#ifndef Userposition_h
#define Userposition_h

//Holds up to 50 stockpositions for a single user 

#include <string>
#include <map>

class UserPosition
{
public:
	UserPosition();
	UserPosition(std::string user);

	long int FindPosition(std::string sTicker);
	void AddNewPosition(std::string sTicker, int iAmount, char cBS);

private:
	int m_iTickers;
	std::string m_sUser;
	std::map<std::string, long int> m_StockPortfolio; //Cheating a bit by using map instead of an array of stockposition objects
};

#endif