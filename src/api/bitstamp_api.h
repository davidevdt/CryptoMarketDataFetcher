#pragma once 

#include "api.h"
#include "web_requests.h"
#include <sstream> 
#include <vector> 
#include <algorithm> 
#include "../json_reader/json_reader.h"
#include "../json_reader/multi_json_reader.h"

using DataMap = std::unordered_map<std::string, std::string>; 
using DataMapVec = std::vector<std::unordered_map<std::string, std::string>>; 

/*
 * Implementation of the Api interface using the Bitstamp exchange RESTful service. 
 * When initialized, it retrieves the list of all tickers available in the exchange. 
 */

class BitstampApi : public Api {

public: 

    // Constructors
    BitstampApi(){retrieveAllTickers();} 
    BitstampApi(std::string n): n_(n){retrieveAllTickers();}
    BitstampApi(int maxConnectionTime): maxConnectionTime_(maxConnectionTime) {
        httpRequestsHandler.setMaxConnectionTime(maxConnectionTime_);
        retrieveAllTickers();
    }

    // void debug() override {std::cout << "Api: " << n_ << std::endl;} 

    // Max connection time getter and setter - Gets and sets the max time to wait for a web request 
    int getMaxConnectionTime() const override {return httpRequestsHandler.getMaxConnectionTime();}
    void setMaxConnectionTime(int maxConnectionTime) override {
        maxConnectionTime_ = maxConnectionTime; 
        httpRequestsHandler.setMaxConnectionTime(maxConnectionTime_); 
    }

    /*
     * Helper functions - They recover the output of the Api web requests as strings.
     * Their output will be passed to the functions overriding the Api interface, which
     * will simply convert the strings into DataMap or DataMapVec objects.
     */
    std::string fetchCurrencyDataString() const; 
    std::string fetchAllPairsString() const; 
    std::string fetchMarketTickerString(const std::string& ticker) const; 
    std::string fetchHourlyTickerString(const std::string& ticker) const; 
    std::string fetchCandlestickDataString(const std::string& ticker, const std::unordered_map<std::string,std::string>& otherArgs) const; 
    std::string fetchEurUsdConversionRateString();  

    /*
     * Functions overriding the Api interface. 
     */
    DataMapVec fetchCurrencyData() override;
    DataMapVec fetchAllPairs() override; 
    DataMap fetchMarketTicker(const std::string& ticker) override; 
    DataMap fetchHourlyTicker(const std::string& ticker) override; 
    DataMapVec fetchCandlestickData(const std::string& ticker, const std::unordered_map<std::string,std::string>& otherArgs) override;
    DataMap fetchEurUsdConversionRate(); 
    std::vector<std::string> fetchAllTickers() override; 
    std::string makePair(const std::string& cryptoSymbol, const std::string& fiatSymbol) const override;
    bool validatePair(const std::string& pair) const override; 


    // Helper function for fechAllTickers()
    void retrieveAllTickers(); 

private:
    HttpRequest httpRequestsHandler{};
    JsonReader jReader{};
    MultiJsonReader multiJsonReader{};  
    int maxConnectionTime_ = httpRequestsHandler.getMaxConnectionTime(); 
    std::vector<std::string> allTickers; 
    std::string n_; 

    // Api URLs: 
    const std::string BASE_URL = "https://www.bitstamp.net/api/v2/"; 
    const std::string CURRENCIES_URL = BASE_URL + "currencies/"; 
    const std::string PAIR_URL = BASE_URL + "ticker/"; // note: without a ticker symbol after it, it retrieves all tickers
    const std::string HOURLY_URL = BASE_URL + "ticker_hour/"; 
    const std::string OHLC_URL = BASE_URL + "ohlc/"; 
    const std::string EUR_USD_URL = BASE_URL + "eur_usd/"; 
};