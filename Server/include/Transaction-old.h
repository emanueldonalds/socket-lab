#ifndef Transaction_H
#define Transaction_H
#include <iostream>
#include <string>

class Transaction
{
public:
	Transaction();
	Transaction(std::string sUser, char m_cBS);
	
	~Transaction();

	std::string GetUser();

	friend std::ostream& operator<<(std::ostream& out, Transaction& tr);
	friend std::istream& operator>>(std::istream& in, Transaction& tr);
private:
	std::string m_sUser;
	char m_cBS;
};
#endif