#include "crypto.h"
#include <unordered_map>

void CryptoDataUpdater::updateMarketData() {
    lastMarketData = apiRequester_->fetchMarketTicker(pair_); 
}

std::string CryptoDataUpdater::fetchMarketData(const std::string& field) {
    if (lastMarketData.empty()) updateMarketData(); 
    return lastMarketData[field];
}

MarketData CryptoDataUpdater::fetchMarketData(const std::vector<std::string>& fields) {
    if (lastMarketData.empty()) updateMarketData(); 
    if (fields.size() == 0) return lastMarketData;
    MarketData filteredMarketData; 
    for (const auto& m: lastMarketData) {
        if (std::find(fields.begin(), fields.end(), m.first) != fields.end()) 
            filteredMarketData[m.first] = m.second;
    }
    return filteredMarketData; 
}

std::vector<MarketData> CryptoDataUpdater::fetchCandlestickData(
    const std::unordered_map<std::string,std::string>& args, 
    std::vector<std::string> fields
) {
    auto candlestickData = apiRequester_->fetchCandlestickData(pair_, args);
    std::vector<MarketData> retData(candlestickData.size());   
    for (size_t i = 0; i < candlestickData.size(); ++i) {
        if (fields.size() == 0) retData.at(i) = candlestickData.at(i); 
        else {
            MarketData filteredMarketData; 
            for (const auto& m: candlestickData.at(i)) {
                if (std::find(fields.begin(), fields.end(), m.first) != fields.end()) 
                    filteredMarketData[m.first] = m.second; 
            }
            retData.at(i) = filteredMarketData;  
        } 
    }
    return retData; 
}