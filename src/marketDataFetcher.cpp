/*
 * File: marketDataFetcher.cpp
 * Description: This program fetches the most recent crypto market data in real-time (up to connection latency), 
 *              and refreshes them at regular intervals. 
 *              The names of the crypto assets whose information wants to be fetched are read from file;
 *              the connections occur through the Bitstamp exchange public Api.
 * Author: Davide Vidotto
 * Date: 2024-07-07
 */

#include "api/api.h"
#include "api/bitstamp_api.h"
#include "crypto_market_data/market_data_fetcher.h"
#include "utils/utils.h"
#include <exception>
#include <memory>
#include <stdexcept>
#include <string> 
#include <memory.h> 

/*
 *  The main function can read 0 to 3 optional arguments: 
 *      - the first one is the path of the file containing the coin names for wich we want to fetch the market data
 *      - the second one is the optional fiat currency name against which the crypto is valuated, defalts to "USD"
 *      -the third one is the wait time which specifies the number of seconds to wait for the next data refresh 
 */
int main (int argc, char** argv) {

    std::cout << std::endl; 

    // Read inputs or use default arguments
    std::string cryptoNamesFilePath; 
    std::string fiatName; 
    int wait_time; 

    cryptoNamesFilePath = argc > 1 ? std::string(argv[1]) : "./config/crypto_names.txt";
    fiatName = argc > 2 ? std::string(argv[2]) : "USD"; 
    try {
        wait_time = argc > 3 ? std::stoi(argv[3]) : 5; 
    }
    catch (const std::exception& e) {
        std::cerr << "Invalid wait time (please specify an integer)." << std::endl; 
        return 1; 
    }

    // Import crypto names from file
    std::vector<std::string> cryptoNames;
    try {
        cryptoNames = Utils::readTxtLines(cryptoNamesFilePath); 
    } 
    catch (const std::runtime_error& e) {
        std::cerr << "Crypto file path not valid." << std::endl; 
        return 1; 
    }

    // Create Api request handlers -- in this case, from Bitstamp Api service
    std::vector<std::unique_ptr<Api>> apiRequesters; 
    for (size_t i=0; i < cryptoNames.size(); ++i) {
        apiRequesters.push_back(std::make_unique<BitstampApi>(wait_time)); 
    }

    // Create the market data fetcher object and fetch the data
    MarketDataFetcher marketDataFetcher; 
    marketDataFetcher.fetchMultiCoinMarketData(cryptoNames, apiRequesters, "timestamp", {}, fiatName); 

    return 0; 
}
