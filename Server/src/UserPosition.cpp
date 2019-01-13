#include "UserPosition.h"
#include <string>
#include <iostream>

UserPosition::UserPosition()
{
	m_iTickers = 0;
}
UserPosition::UserPosition(std::string user)
{
	m_iTickers = 0;
	m_sUser = user;
}

long int UserPosition::FindPosition(std::string sTicker) //finds the position of a given ticker
{
	return m_StockPortfolio[sTicker];
}

void UserPosition::AddNewPosition(std::string sTicker, int iAmount, char cBS)
{
	if (m_StockPortfolio.size() <= 50) //store up to 50 positions
	{
		if (cBS == 'b')
			m_StockPortfolio[sTicker] += iAmount;
		else if (cBS == 's')
			m_StockPortfolio[sTicker] -= iAmount;
	}
	else //if 50 positions are taken, only update if the ticker already exists
	{
		std::map<std::string, long int>::iterator it;
		it = m_StockPortfolio.find(sTicker);
		if (it != m_StockPortfolio.end())
		{
			if (cBS == 'b')
				m_StockPortfolio[sTicker] += iAmount;
			else if (cBS == 's')
				m_StockPortfolio[sTicker] -= iAmount;
		}
	}
	m_iTickers = m_StockPortfolio.size();
}