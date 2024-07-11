#include "market_data_fetcher.h"
#include <memory>
#include <stdexcept>
#include <utility>

std::atomic<bool> MarketDataFetcher::terminateFlag{false};
std::atomic<bool> MarketDataFetcher::terminateInnerLoopFlag{false};
std::atomic<bool> MarketDataFetcher::terminateOuterLoopFlag{false};

// For a specific crypto asset, it fetches the real time (most recent) market data
void MarketDataFetcher::fetchCoinMarketData(
    const std::string& name, 
    const std::unique_ptr<Api>& apiRequester, 
    const std::string& timestampField,
    const std::vector<std::string>& fields,
    const std::string& fiat
) {
    try {
        CryptoDataUpdater crypto(name, fiat, *apiRequester); 
        std::unordered_map<std::string, std::string> marketData;  
        std::string lastTimestamp{""}; 
    
        while (!terminateFlag.load()) {
            crypto.updateMarketData(); 
            marketData = crypto.fetchMarketData(); 
            if (lastTimestamp != marketData[timestampField]) {
                lastTimestamp = marketData[timestampField]; 
                {
                    std::lock_guard<std::mutex> lock(coutMutex); 
                    std::cout << Utils::mapToMessage(name + "/" + fiat, marketData, fields).str() << std::endl; 
                    std::cout << std::string(50, '-') << std::endl; 
                }
            }
            std::this_thread::sleep_for(std::chrono::seconds(WAIT_TIME)); 
        } 
    } 
    catch(std::invalid_argument) {
        std::cout << name << " : invalid coin name." << std::endl; 
        return; 
    }

    std::cout << name << ": thread terminated." << std::endl; 
}

// It calls asynchronously the fetchCoinMarketData method, 
// to handle multiple crypto-assets requests simultaneosly. 
void MarketDataFetcher::fetchMultiCoinMarketData(
    const std::vector<std::string>& cryptoNames,
    const std::vector<std::unique_ptr<Api>>& apiRequesters,
    const std::string& timestampField,
    const std::vector<std::string>& fields, 
    const std::string& fiat
) {
    if (cryptoNames.size() != apiRequesters.size())
        throw std::runtime_error("Number of cryptos and number of api request handlers must be equal.");

    std::vector<std::thread> threads; 

    for (int i = 0; i < cryptoNames.size(); ++i) {
        threads.emplace_back([this, &cryptoNames, i, &apiRequesters, &fiat, &timestampField, &fields]() {
            fetchCoinMarketData(cryptoNames.at(i), apiRequesters.at(i), timestampField, fields, fiat
        );}); 
    }
    for (auto& t: threads) t.join(); 
}

// It fetches candlestick data for a specific crypto assets, and send it to the calling thread. 
// It notifies the calling thread when the corresponding data are ready. 
void MarketDataFetcher::fetchAndSendCoinCandlestickData (
    const std::string& name, 
    const int threadNumber, 
    const std::unique_ptr<Api>& apiRequester, 
    std::vector<bool>& threadsReady, 
    std::unordered_map<std::string, std::vector<std::unordered_map<std::string, std::string>>>& data, 
    const std::unordered_map<std::string, std::string>& ohlcArgs,
    const std::vector<std::string>& fields, 
    const std::string& fiat, 
    bool notifyEnd
) {
    try {
        CryptoDataUpdater crypto(name, fiat, *apiRequester); 
    
        while (!terminateInnerLoopFlag.load()) {
            if (!threadsReady.at(threadNumber)) {
                data[name] = crypto.fetchCandlestickData(ohlcArgs, fields); 

                // std::lock_guard<std::mutex> l(mapMutex); 
                threadsReady.at(threadNumber) = true; 
                {
                    std::lock_guard<std::mutex> dataReadyLock(dataReadyMutex);
                    dataReady.notify_all(); 
                }
                std::this_thread::sleep_for(std::chrono::seconds(WAIT_TIME)); 
            }
        } 
    } 
    catch(std::invalid_argument) {
        std::cout << name << " : invalid coin name." << std::endl; 
        {
            threadsReady.at(threadNumber) = true; 
            std::lock_guard<std::mutex> dataReadyLock(dataReadyMutex);
            dataReady.notify_all(); 
        }
        // terminateInnerLoopFlag.store(true); 
        // return; 
    }

    std::cout << name << ": thread terminated." << std::endl; 
    if (notifyEnd) {
        std::lock_guard<std::mutex> dataReadyLock(dataReadyMutex);
        dataReady.notify_all(); 
    }
}

// It fetches and prints to screen the candlestick data for a specific crypto asset. 
// If csvFormat = false, it returns the data in tabular form. 
void MarketDataFetcher::fetchAndPrintCoinCandlestickData (
    const std::string& name, 
    const std::unique_ptr<Api>& apiRequester, 
    const std::unordered_map<std::string, std::string>& ohlcArgs,
    const std::vector<std::string>& fields, 
    const std::string& timestampField, 
    const std::string& fiat, 
    bool csvFormat
) {
    try {
        CryptoDataUpdater crypto(name, fiat, *apiRequester); 
    
        while (!terminateInnerLoopFlag.load()) {

            auto data = crypto.fetchCandlestickData(ohlcArgs, fields); 

            {
                std::lock_guard<std::mutex> lock(coutMutex); 
                std::cout << Utils::formatMapVector(data, name + '/' +  fiat + '-', timestampField, {}, csvFormat) << std::endl; 
                std::cout << std::string(15 * fields.size(), '-') << std::endl; 
            }
            std::this_thread::sleep_for(std::chrono::seconds(WAIT_TIME)); 
        } 
    } 
    catch(std::invalid_argument) {
        std::cout << name << " : invalid coin name." << std::endl; 
        terminateInnerLoopFlag.store(true); 
        return; 
    }

    std::cout << name << ": thread terminated." << std::endl; 
}

// It calls fetchAndSendCoinCandlestickData to fetch and gather candlestick data
// about multiple crypto assets. Importantly, only a single candlestick field 
// (besides the timestamp) is fetched (for example, the volume or close price). 
// Such data are then printed to screen in tabular form. 
void MarketDataFetcher::fetchMultiCoinSingleCandlestickField(
    const std::vector<std::string>& cryptoNames, 
    const std::vector<std::unique_ptr<Api>>& apiRequesters, 
    const std::unordered_map<std::string, std::string>& ohlcArgs,
    const std::string& timestampField, 
    const std::string& candlestickField, 
    const std::string& fiat
) {

    if (cryptoNames.size() != apiRequesters.size())
        throw std::runtime_error("Number of cryptos and number of api request handlers must be equal.");

    std::vector<std::string> fields{timestampField, candlestickField}; 
    std::unordered_map<std::string, std::vector<std::unordered_map<std::string, std::string>>> data; 
    std::vector<bool> threadsReady(cryptoNames.size(), false); 
    std::vector<std::thread> threads; 
    
    for (int i = 0; i < cryptoNames.size(); ++i) {
        threads.emplace_back([this, cryptoNames, i, &threadsReady, &apiRequesters, &data, &ohlcArgs, &fiat, &fields]() {
            fetchAndSendCoinCandlestickData(
                cryptoNames.at(i), i, apiRequesters.at(i), threadsReady, 
                data, ohlcArgs, fields, fiat, true);
        }); 
    }

    while (!terminateOuterLoopFlag.load()) {

        std::unique_lock<std::mutex> lock(dataReadyMutex);
        dataReady.wait(lock, [this, &threadsReady]() {
            return std::all_of(threadsReady.begin(), threadsReady.end(), [](bool v) { return v; }) || terminateInnerLoopFlag; 
        });

        if (terminateInnerLoopFlag) break; 

        // Only print to screen when all results are ready 
        std::vector<std::string> timestampVector; 
        std::vector<std::vector<std::string>> values(cryptoNames.size());

        // Convert maps into vector of vectors
        size_t i = 0; 
        for (const auto& n: cryptoNames) {
            auto valuesMap = data[n]; 
            if (valuesMap.empty()) continue;
            std::vector<std::string> tmp; // contains prices data of one specific coin
            for (auto& v: valuesMap) {
                tmp.push_back(v[candlestickField]); 
                if (i == 0) timestampVector.push_back(Utils::timestampToString(std::stoi(v[timestampField]))); 
            }
            values.at(i) = tmp; 
            ++i; 
        }

        // Print the data 
        std::cout << Utils::matrixToMsg(cryptoNames, timestampVector, values).str() << std::endl; 

        std::fill(threadsReady.begin(), threadsReady.end(), false); 
        if (terminateInnerLoopFlag.load()) terminateOuterLoopFlag.store(true); 
    }

    for (auto& t: threads) t.join(); 
    return; 
}

// It calls fetchAndSendCoinCandlestickData to fetch candlestick data about
// multiple crypto assets. Subsequently, it prints to screen the candlestick data 
// and, on requests, it saves them into csv format if csvFilePath is specified. 
// Data are downloaded once, after which all the threads are terminated. 
void MarketDataFetcher::downloadMultiCoinCandlestickData(
    const std::vector<std::string>& cryptoNames, 
    const std::vector<std::unique_ptr<Api>>& apiRequesters, 
    const std::unordered_map<std::string, std::string>& ohlcArgs,
    const std::string& timestampField, 
    const std::string& csvFilePath, 
    const std::vector<std::string>& fields, 
    const std::string& fiat
) {

    if (cryptoNames.size() != apiRequesters.size())
        throw std::runtime_error("Number of cryptos and number of api request handlers must be equal.");

    std::unordered_map<std::string, std::vector<std::unordered_map<std::string, std::string>>> data; 
    std::vector<bool> threadsReady(cryptoNames.size(), false); 
    std::vector<std::thread> threads; 
    
    for (int i = 0; i < cryptoNames.size(); ++i) {
        threads.emplace_back([this, cryptoNames, i, &threadsReady, &apiRequesters, &data, &ohlcArgs, &fiat, &fields]() {
            fetchAndSendCoinCandlestickData(
                cryptoNames.at(i), i, apiRequesters.at(i), threadsReady, 
                data, ohlcArgs, fields, fiat, false);
        }); 
    }

    std::unique_lock<std::mutex> lock(dataReadyMutex);
    dataReady.wait(lock, [&threadsReady]() {
        return std::all_of(threadsReady.begin(), threadsReady.end(), [](bool v) { return v; }); 
    });

    terminateInnerLoopFlag.store(true);    
    for (auto& t: threads) t.join(); 

    std::cout << std::endl; 
    
    for (const auto& marketData: data) {

        auto name = marketData.first; 
        std::cout << Utils::formatMapVector(marketData.second, name + '/' + fiat + '_', timestampField, {}, false) << std::endl; 
        std::cout << std::string(15 * fields.size(), '-') << std::endl; 

        if (csvFilePath != "") {
            auto fileName = csvFilePath; 
            auto out = Utils::formatMapVector(marketData.second, name + '/' + fiat +  '_', timestampField, {}, true); 
            if (csvFilePath.back() != '/') fileName += '/'; 
            fileName += name + '_' + fiat + '_' + Utils::timestampToString(std::stoi(marketData.second.back().at(timestampField))) + ".csv"; 
            if (Utils::writeStringToFile(out, fileName) == 1)  
                std::cout << fileName << " written to disk.\n" << std::endl; 
        }
    }
    return; 
}

// It calls fetchAndPrintCoinCandlestickData to print the candlestick data about multiple 
// crypto assets.  
void MarketDataFetcher::fetchMultiCoinCandlestickData(
    const std::vector<std::string>& cryptoNames, 
    const std::vector<std::unique_ptr<Api>>& apiRequesters, 
    const std::unordered_map<std::string, std::string>& ohlcArgs,
    const std::string& timestampField, 
    const std::vector<std::string>& fields, 
    const std::string& fiat, 
    bool csvFormat 
) {

    if (cryptoNames.size() != apiRequesters.size())
        throw std::runtime_error("Number of cryptos and number of api request handlers must be equal.");

    std::vector<std::thread> threads; 
    
    for (int i = 0; i < cryptoNames.size(); ++i) {
        threads.emplace_back([this, cryptoNames, i, &apiRequesters, &ohlcArgs, &fiat, &fields, &timestampField, &csvFormat]() {
            fetchAndPrintCoinCandlestickData(
                cryptoNames.at(i), apiRequesters.at(i), ohlcArgs, fields, timestampField, fiat, csvFormat);
        }); 
    }
    for (auto& t: threads) t.join(); 
}