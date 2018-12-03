#ifndef Transaction_H
#define Transaction_H

//Holds buffer data for transfering stock transactions 

#include <iostream>
#include <string>

class Transaction
{
public:
	Transaction(std::istream &in);

	std::string GetUser();
	std::string GetTicker();
	char GetBS();
	long int GetAmount();
	void UpdateAmount(long int amount);

	void OutputData(std::ostream &out);
private:
	std::string m_sUser;
	std::string m_sTicker;
	char m_cBS;
	long int m_liAmount;
};
#endif