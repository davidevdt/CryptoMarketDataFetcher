#pragma once 

#include "crypto.h"
#include "../api/api.h"
#include "../utils/utils.h"

#include <cstddef>
#include <string> 
#include <thread> 
#include <chrono> 
#include <csignal>  
#include <atomic> 
#include <mutex> 
#include <condition_variable>
#include <algorithm>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <iomanip>
#include <memory> 

/*
 * The class offers user interface functionalities to fetch real-time crypto market data 
 * from a specific exchange Api handler. Most of the inputs, such as crypto names, etc, 
 * are passed to the class' methods, rather than the constructor. To handle multiple requests 
 * simultaneously, the class makes use of multithreading.  
 */
class MarketDataFetcher {
public:

    // Constructors
    MarketDataFetcher() {
        std::signal(SIGINT, sigintHandler);
    } 

    MarketDataFetcher(size_t waitTime) {
        setWaitTime(waitTime); 
        std::signal(SIGINT, sigintHandler);
    } 

    // Wait time in seconds (= number of seconds to wait to refresh the data)
    size_t getWaitTime() const {return WAIT_TIME;}
    void setWaitTime(size_t newWaitTime) {
        if (newWaitTime <= 2) newWaitTime = 3; // We wait at least three second  
        WAIT_TIME = newWaitTime; 
    }

    // Fetches the latest market data for multiple crypto assets
    void fetchMultiCoinMarketData(
        const std::vector<std::string>& cryptoNames,
        const std::vector<std::unique_ptr<Api>>& apiRequesters,
        const std::string& timestampField,
        const std::vector<std::string>& fields = {}, 
        const std::string& fiat = "usd"
    );

    // Fetches a specific field of the candlestick data for multiple crypto assets, 
    // prints them to screen in tabular format, and refreshes them regularly
    void fetchMultiCoinSingleCandlestickField(
        const std::vector<std::string>& cryptoNames, 
        const std::vector<std::unique_ptr<Api>>& apiRequesters, 
        const std::unordered_map<std::string, std::string>& ohlcArgs,
        const std::string& timestampField, 
        const std::string& candlestickField, 
        const std::string& fiat = "usd"
    ); 

    // Downloads the candlestick data for multiple crypto assets
    // The downloaded data are printed to screen and, if the csvFilePath
    // argument is populated, it stores them to file in csv format
    void downloadMultiCoinCandlestickData(
        const std::vector<std::string>& cryptoNames, 
        const std::vector<std::unique_ptr<Api>>& apiRequesters, 
        const std::unordered_map<std::string, std::string>& ohlcArgs,
        const std::string& timestampField, 
        const std::string& csvFilePath = "", 
        const std::vector<std::string>& fields = {}, 
        const std::string& fiat = "usd"
    );
    
    // Fetches the candlestick data for multiple crypto assets, 
    // prints them to screen in tabular/csv format (depending onf the csvFormat parameter), 
    // and refreshes them regularly
    void fetchMultiCoinCandlestickData(
        const std::vector<std::string>& cryptoNames, 
        const std::vector<std::unique_ptr<Api>>& apiRequesters, 
        const std::unordered_map<std::string, std::string>& ohlcArgs,
        const std::string& timestampField = "timestamp", 
        const std::vector<std::string>& fields = {}, 
        const std::string& fiat = "usd", 
        bool csvFormat = true
    ); 

private:

    // Fetches the latest market data for a single crypto asset
    void fetchCoinMarketData(
        const std::string& name, 
        const std::unique_ptr<Api>& apiRequester, 
        const std::string& timestampField,
        const std::vector<std::string>& fields = {},
        const std::string& fiat = "usd"
    );

    // Fetches the latest market data for a single crypto on a separate thread, 
    // and returns the result to the calling thread
    void fetchAndSendCoinCandlestickData(
        const std::string& name, 
        const int threadNumber, 
        const std::unique_ptr<Api>& apiRequester, 
        std::vector<bool>& threadsReady, 
        std::unordered_map<std::string, std::vector<std::unordered_map<std::string, std::string>>>& data, 
        const std::unordered_map<std::string, std::string>& ohlcArgs,
        const std::vector<std::string>& fields = {},
        const std::string& fiat = "usd", 
        bool notifyEnd = false
    ); 

    // Fetches the latest market data for a single crypto on a separate thread, 
    // and prints the results to screen
    void fetchAndPrintCoinCandlestickData (
        const std::string& name, 
        const std::unique_ptr<Api>& apiRequester, 
        const std::unordered_map<std::string, std::string>& ohlcArgs,
        const std::vector<std::string>& fields = {}, 
        const std::string& timestampField = "timestamp", 
        const std::string& fiat = "usd", 
        bool csvFormat = true 
    ); 

    // Signal Handler for Ctrl+C - It will terminate threads
    static void sigintHandler(int signal) {
        if (signal == SIGINT) {
            std::cout << " Ctrl+C detected. Terminating program, please wait...\n";
            terminateFlag.store(true);
            terminateInnerLoopFlag.store(true);
        }
    }

    static std::atomic<bool> terminateFlag;
    static std::atomic<bool> terminateInnerLoopFlag;
    static std::atomic<bool> terminateOuterLoopFlag;

    std::condition_variable dataReady;
    std::mutex coutMutex; 
    std::mutex mapMutex; 
    std::mutex dataReadyMutex;

    size_t WAIT_TIME = 10; // waiting time in seconds 

}; 