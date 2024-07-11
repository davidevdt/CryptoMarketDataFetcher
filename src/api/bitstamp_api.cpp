#include "bitstamp_api.h"
#include "api.h"
#include <cctype>

std::string BitstampApi::fetchCurrencyDataString() const {
    return httpRequestsHandler.request(CURRENCIES_URL); 
}

std::string BitstampApi::fetchAllPairsString() const {
    return httpRequestsHandler.request(PAIR_URL); 
} 

std::string BitstampApi::fetchMarketTickerString(const std::string& ticker) const {
    return httpRequestsHandler.request(PAIR_URL + ticker);
} 


std::string BitstampApi::fetchHourlyTickerString(const std::string& ticker) const {
    return httpRequestsHandler.request(HOURLY_URL + ticker); 
} 

std::string BitstampApi::fetchCandlestickDataString(const std::string& ticker, const std::unordered_map<std::string,std::string>& otherArgs) const {
    std::stringstream ss; 
    size_t count = 0; 
    for (const auto& args: otherArgs) {
        ss << args.first << '=' << args.second; 
        if (++count != otherArgs.size()) ss << "&"; 
    }
    return httpRequestsHandler.request(OHLC_URL + ticker + "/?" + ss.str());
} 

std::string BitstampApi::fetchEurUsdConversionRateString() {
    return httpRequestsHandler.request(EUR_USD_URL);
} 

DataMapVec BitstampApi::fetchCurrencyData() {
    multiJsonReader.setFromString(this->fetchCurrencyDataString()); 
    return multiJsonReader.get(); 
}

DataMapVec BitstampApi::fetchAllPairs() {
    multiJsonReader.setFromString(this->fetchAllPairsString()); 
    return multiJsonReader.get(); 
}

DataMap BitstampApi::fetchMarketTicker(const std::string& ticker) {
    jReader.setFromString(this->fetchMarketTickerString(ticker)); 
    return jReader.get(); 
} 

DataMap BitstampApi::fetchHourlyTicker(const std::string& ticker) {
    jReader.setFromString(this->fetchHourlyTickerString(ticker)); 
    return jReader.get(); 
}

DataMapVec BitstampApi::fetchCandlestickData(const std::string& ticker, const std::unordered_map<std::string,std::string>& otherArgs) {
    multiJsonReader.setFromString(this->fetchCandlestickDataString(ticker, otherArgs)); 
    return multiJsonReader.get(); 
}

DataMap BitstampApi::fetchEurUsdConversionRate() {
    jReader.setFromString(this->fetchEurUsdConversionRateString()); 
    return jReader.get(); 
}

void BitstampApi::retrieveAllTickers() {
    allTickers.clear(); 
    for (auto& pair: this->fetchAllPairs()) {
        auto ticker = pair["pair"]; 
        std::string newTicker; 
        for (const auto c: ticker) {
            if (c != '/') newTicker += std::tolower(c);
        }
        allTickers.push_back(newTicker); 
    }
}

std::vector<std::string> BitstampApi::fetchAllTickers() {
    if (allTickers.empty()) this->retrieveAllTickers(); 
    return allTickers; 
} 

std::string BitstampApi::makePair(const std::string& cryptoSymbol, const std::string& fiatSymbol) const {
    std::string pair; 
    for (const auto c: cryptoSymbol) {
        pair += std::tolower(c); 
    } 
    for (const auto f: fiatSymbol) {
        pair += std::tolower(f); 
    }
    return pair; 
}

bool BitstampApi::validatePair(const std::string& pair) const {
    if (!allTickers.empty() && std::find(allTickers.begin(), allTickers.end(), pair) != allTickers.end()) {
        return true; 
    }
    return false; 
}