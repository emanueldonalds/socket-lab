#include "Transaction.h"
#include <string>
#include <iostream>

Transaction::Transaction(std::istream &in)
{
	in >> m_sUser >> m_sTicker >> m_cBS >> m_liAmount;
}

std::string Transaction::GetUser()
{
	return m_sUser;
}

std::string Transaction::GetTicker()
{
	return m_sTicker;
}
char Transaction::GetBS()
{
	return m_cBS;
}
long int Transaction::GetAmount()
{
	return m_liAmount;
}

void Transaction::UpdateAmount(long int amount)
{
	m_liAmount = amount;
}

void Transaction::OutputData(std::ostream &out)
{
	out << m_sUser << " " << m_sTicker << " " << m_cBS << " " << m_liAmount;
}