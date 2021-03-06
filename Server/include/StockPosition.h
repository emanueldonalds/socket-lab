//Holds data about a single stock position

#ifndef Stockposition_H
#define Stockposition_H

#include <string>

class StockPosition
{
public:
	StockPosition();
	StockPosition(std::string& ticker);
	void Update(long int &position, char& c);
	std::string Ticker();
private:
	std::string m_sTicker;
	long int m_liPosition;

};

#endif