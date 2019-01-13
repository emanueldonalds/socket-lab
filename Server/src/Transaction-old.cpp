#include <iostream>
#include <string>
#include "Transaction.h"


Transaction::Transaction()
{

}
Transaction::Transaction(std::string sUser, char cBS)
{
	m_sUser = sUser;
	m_cBS = cBS;
}

Transaction::~Transaction()
{

}

std::ostream& operator<<(std::ostream& out, Transaction& tr)
{
	out << tr.m_sUser << ' ' << tr.m_cBS ;
	return out;
}

std::istream& operator>>(std::istream& in, Transaction& tr)
{
	in >> tr.m_sUser;
	in >> tr.m_cBS;
	return in;
}

std::string Transaction::GetUser()
{
	return m_sUser;
}

