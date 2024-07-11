/*
 * File: candlestickDataDownloader.cpp
 * Description: This program downloads the most recent crypto candlestick data in real-time (up to connection latency); 
 *              after this, it prints the data and on requests it saves them to csv files.
 *              The names of the crypto assets whose information wants to be fetched are read from file;
 *              the connections occur through the Bitstamp exchange public Api.
 * Author: Davide Vidottomkdir build
cd build
cmake ..
make
 * Date: 2024-07-07
 */

#include "api/api.h"
#include "api/bitstamp_api.h"
#include "crypto_market_data/market_data_fetcher.h"
#include "json_reader/json_reader.h"
#include "utils/utils.h"
#include <exception>
#include <memory>
#include <stdexcept>
#include <string> 
#include <memory.h> 
#include <unordered_map>

/*
 *  The main function can read 0 to 5 optional arguments: 
 *      - the first one is the path of the file containing the coin names for wich we want to fetch the market data
 *      - the second one is the optional path where the csv files can be stored; 
 *        specify "" or '' if you don't want to save the data into csv files
 *      - the third one is the optional fiat currency name against which the crypto is valuated, defalts to "USD"
 *      - the fourth one is the wait time which specifies the number of seconds to wait for the next data refresh 
 *      - the fifth one is the path of the file containing the options for the candlestick api request (Api/exchange-dependent) 
*/
int main (int argc, char** argv) {

    std::cout << std::endl; 

    // Read inputs or use default arguments
    std::string cryptoNamesFilePath; 
    std::string outputFilesPath; 
    std::string fiatName; 
    int wait_time; 
    std::unordered_map<std::string, std::string> ohlcParams; 

    cryptoNamesFilePath = argc > 1 ? std::string(argv[1]) : "./config/crypto_names.txt";
    outputFilesPath = argc > 2 ? std::string(argv[2]) : "./data/"; 
    fiatName = argc > 3 ? std::string(argv[3]) : "USD"; 
    
    try {
        wait_time = argc > 4 ? std::stoi(argv[4]) : 5; 
    }
    catch (const std::exception& e) {
        std::cerr << "Invalid wait time (please specify an integer)." << std::endl; 
        return 1; 
    }

    try {
        std::string ohlcFilename = argc > 5 ? argv[5] : "./config/ohlc_params.json"; 
        ohlcParams = JsonReader(ohlcFilename, true).get(); 
    }
    catch (const std::exception& e) {
        std::cerr << "Could not read the OHLC parameters from " << argv[4] << std::endl; 
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

    // Create the market data fetcher object and download the data
    std::cout << "Downloading the data. Please wait...\n" << std::endl; 

    MarketDataFetcher marketDataFetcher; 
    marketDataFetcher.downloadMultiCoinCandlestickData(cryptoNames, apiRequesters, ohlcParams, "timestamp", outputFilesPath, {}, fiatName); 
    return 0; 
}
