#pragma once

#include <string> 
#include <vector> 
#include <unordered_map> 

using DataMap = std::unordered_map<std::string, std::string>; 
using DataMapVec = std::vector<std::unordered_map<std::string, std::string>>; 

/*
 *  Interface containing base methods for crypto data retrieval from any generic exchange. 
 *  Concrete implementations of this class will define the methods to get the data from a 
 *  specific exchange api. 
 */
class Api {

public:
    // Max connection time: max time (in seconds) to wait for an Api request. 
    virtual int getMaxConnectionTime() const = 0; 
    virtual void setMaxConnectionTime(int maxConnectionTime) = 0; 

    // virtual void debug() = 0; 

    virtual DataMapVec fetchCurrencyData() = 0; // gets data about all currencies in the exchanges
    virtual DataMapVec fetchAllPairs() = 0; // gets information about all pairs in the exchange
    virtual DataMap fetchMarketTicker(const std::string& ticker) = 0; // gets the latest market data for a specific ticker 
    virtual DataMap fetchHourlyTicker(const std::string& ticker) = 0; // gets hourly market data for a specific ticker
    
    // Gets candlestick data about a specific ticker -
    // otherArgs is a map in which the keys denote the request parameter names, and the values are the request parameter values
    virtual DataMapVec fetchCandlestickData(const std::string& ticker, const std::unordered_map<std::string,std::string>& otherArgs) = 0;

    virtual std::vector<std::string> fetchAllTickers() = 0; // gets all ticker names (all pairs)
    
    // Given a crypto name and a fiat (or other conversion currency), it creates a pair name
    // according to the exchange's taxonomy
    virtual std::string makePair(const std::string& cryptoSymbol, const std::string& fiatSymbol) const = 0; 
    
    // Given a pair name, it checks if it is a valid pair for the exchange
    virtual bool validatePair(const std::string& pair) const = 0; 

    virtual ~Api() {}
}; 