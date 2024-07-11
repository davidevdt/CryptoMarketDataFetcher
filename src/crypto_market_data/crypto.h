#pragma  once 

#include "../api/api.h"
#include <algorithm>
#include <unordered_map>
#include "../json_reader/json_reader.h"
#include "../json_reader/multi_json_reader.h"

using MarketData = std::unordered_map<std::string, std::string>; 

/*
 * The class is initialized by specifying a Crypto symbol (e.g., "BTC" for Bitcoin) and an Api request handler
 * that makes the Api requests. The class contains methods that update the market data of the specified 
 * cryptocurrency and other that fetch its candlestick data. 
 * NOTE: make sure that the symbol used for the crypto name (as well as the corresponding conversion currency,
 * here simply called fiat, which completes the pair name) appear in the corresponding exchange 
 * from which the Api requests are performed. If not, an "Invalid Argument" exception is thrown 
 * with the class constructor. 
 */
class CryptoDataUpdater {

public: 
    // Constructors
    CryptoDataUpdater() = delete; // the Api request handler and the crypto name must always be specified 

    CryptoDataUpdater(std::string name, Api &apiRequester, int maxConnectionTime):
        name_(name), apiRequester_(&apiRequester), maxConnectionTime_(maxConnectionTime) {
            apiRequester_->setMaxConnectionTime(maxConnectionTime_); 
            pair_ = apiRequester_->makePair(name_, fiat_);  
            if (!apiRequester_->validatePair(pair_)) {
                throw std::invalid_argument("Invalid input: " + pair_ + " not among the API tickers."); 
            }
    }

    CryptoDataUpdater(std::string name, Api &apiRequester):
        name_(name), apiRequester_(&apiRequester), maxConnectionTime_(apiRequester.getMaxConnectionTime()) {
            apiRequester_->setMaxConnectionTime(maxConnectionTime_); 
            pair_ = apiRequester_->makePair(name_, fiat_);  
            if (!apiRequester_->validatePair(pair_)) {
                throw std::invalid_argument("Invalid input: " + pair_ + " not among the API tickers."); 
            }
    }

    CryptoDataUpdater(std::string name, std::string fiat, Api& apiRequester, int maxConnectionTime):
        name_(name), fiat_(fiat), apiRequester_(&apiRequester), maxConnectionTime_(maxConnectionTime) {
            apiRequester_->setMaxConnectionTime(maxConnectionTime_); 
            pair_ = apiRequester_->makePair(name_, fiat_); 
            if (!apiRequester_->validatePair(pair_)) {
                throw std::invalid_argument("Invalid input: " + pair_ + " not among the API tickers."); 
            }
    } 

    CryptoDataUpdater(std::string name, std::string fiat, Api& apiRequester):
        name_(name), fiat_(fiat), apiRequester_(&apiRequester), maxConnectionTime_(apiRequester.getMaxConnectionTime()) {
            apiRequester_->setMaxConnectionTime(maxConnectionTime_); 
            pair_ = apiRequester_->makePair(name_, fiat_); 
            if (!apiRequester_->validatePair(pair_)) {
                throw std::invalid_argument("Invalid input: " + pair_ + " not among the API tickers."); 
            }
    } 

    CryptoDataUpdater(const CryptoDataUpdater&) = delete; 
    CryptoDataUpdater(const CryptoDataUpdater&&) = delete;
    CryptoDataUpdater& operator=(const CryptoDataUpdater&) = delete; 
    CryptoDataUpdater& operator=(const CryptoDataUpdater&&) = delete; 
    ~CryptoDataUpdater() {} // The object does NOT own the Api requester pointer, so it won't be deleted here

    // Max connection time (= max wait time for an api request) getter and setter
    int getMaxConnectionTime() const {return maxConnectionTime_;} 
    void setMaxConnectionTime(int maxConnectionTime) {
        if (maxConnectionTime < 0) maxConnectionTime  = -1; 
        maxConnectionTime_ = maxConnectionTime;
        apiRequester_->setMaxConnectionTime(maxConnectionTime_); 
    }

    // Api request handler getter
    Api* getApiRequester() const {return apiRequester_;}

    void updateMarketData(); // updates the lastMarketData object
    std::string fetchMarketData(const std::string& field); // fetches a specific field of the latest market data
    
    // updates and gets the lastMarketData object (returns a map) 
    // The optional fields parameter specifies which market data components to returns 
    MarketData fetchMarketData(const std::vector<std::string>& fields = {});

    // Gets the candlestick data
    // The args parameter specifies the parameters for tha Api web request 
    // The fields parameter specifies which components of the candlestick to return 
    std::vector<MarketData> fetchCandlestickData(
        const std::unordered_map<std::string,std::string>& args, 
        std::vector<std::string> fields = {}
    );

private:
    std::string name_; 
    std::string fiat_ = "USD"; 
    std::string pair_; 

    Api* apiRequester_; 
    int maxConnectionTime_;
    MarketData lastMarketData;  
}; 