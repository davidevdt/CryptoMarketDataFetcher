cmake_minimum_required(VERSION 3.10)

project(CryptoMarketDataFetcher)

# Add subdirectories
add_subdirectory(src/json_reader)
add_subdirectory(src/utils)
add_subdirectory(src/api)
add_subdirectory(src/crypto_market_data)

# Include directories
include_directories(src/json_reader)
include_directories(src/utils)
include_directories(src/api)
include_directories(src/crypto_market_data)

# First program: marketDataFetcher
add_executable(marketDataFetcher src/marketDataFetcher.cpp)
target_link_libraries(marketDataFetcher utils crypto_market_data api json_reader)

# Second program: candlestickDataFetcher
add_executable(candlestickDataFetcher src/candlestickDataFetcher.cpp)
target_link_libraries(candlestickDataFetcher crypto_market_data utils api json_reader)

# Third program: candlestickDataDownloader
add_executable(candlestickDataDownloader src/candlestickDataDownloader.cpp)
target_link_libraries(candlestickDataDownloader crypto_market_data utils api json_reader)

set_target_properties(marketDataFetcher PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin)
set_target_properties(candlestickDataFetcher PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin)
set_target_properties(candlestickDataDownloader PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin)