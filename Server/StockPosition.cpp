#include "StockPosition.h"
#include <string>

StockPosition::StockPosition()
{
	m_liPosition = 0;
}

StockPosition::StockPosition(std::string& ticker)
{
	m_liPosition = 0;
	m_sTicker = ticker;
}

void StockPosition::Update(long int& position, char& c)
{
	if (c == 'b')
		m_liPosition -= position;
	else if (c == 's')
		m_liPosition += position;
}

std::string StockPosition::Ticker()
{
	return m_sTicker;
}