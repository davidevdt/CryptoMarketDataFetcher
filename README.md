# CryptoMarketDataFetcher
A C++ system that allows fetching and downloading the latest crypto market and candlestick data. 

This is a C++ program developed from scratch, that allows fetching real-time the latest crypto market data. 

In particular, the program allows performing three operations:
* fetching and displaying real-time updates of the latest crypto market data, which include price, volume, 24h % change, and more.
* fetching and displaying real-time updates of the latest candlestick data (the last 1000 daily data points by default).
* downloading and printing on screen the last candlestick data, saving it in CSV format within the `./data` folder

The software does not make use of third party libraries, except for the C++ standard library (standard `C++11`). It is written to run on Linux/Unix systems, and it only requires having `curl` installed on the host machine. For the development, I have used `curl 8.8.0`.  

Importantly, this system was created for recreational purposes only, and was by no means devised for trading (especially short-term and high-frequency trading). 

## Architecture overview
The `src` folder contains four modules:
* `utils`, which contains methods for the conversion of hash table into strings, of strings into tabular and csv formats, file export, etc.
* `json_reader`, which contains two classes that parse strings containing json objects and convert them into hash tables (`std::unordered_map` in the standard library): one class parses simple json objects, while the other parses vectors of json objects. 
* `api`, which contains the Api interface (`api.h`), which declares methods for fetching the data from the exchange; concrete implementations of this abstract class are exchange-dependent. This folder also contains an example of such implementation, where I implement the class using the public Bitstamp exchange Api (<"https://www.bitstamp.net/api/">). The `web_requests.h` file contains the class responsible for performing the actual web requests. Notice that these are RESTful (and not socket) requests.
* `crypto_market_data` contains an example of how the Api class could be used: `crypto.h` defines a class responsible for fetching the data of a specific crypto asset, while `market_data_fetcher.h` fetches such data for multiple crypto asset simultaneously, through multi-threading. 

The files `marketDataFetcher.cpp`, `candlestickDataFetcher.cpp`, and `candlestickDataDownloader.cpp` in the `src` folder contain the source code for the executables. Of course, these (and the `crypto_market_data` folder) are only examples of how the functionalities of the Api interface can be used. 

The Api interface can be easily extended for use with any exchange; it suffices to override its method and adapt them to the specific exchange Api. 

## Compiling and Running the Program
To download and compile the program:
```
git clone https://github.com/davidevdt/CryptoMarketDataFetcher.git
cd CryptoMarketDataFetcher
./compile.sh
```

The executables will be placed in the `bin` folder. To run, for example, marketDataFetcher:
```
./bin/marketDataFetcher
```

See the source file for more information about the parameters that can be given when launching the program. 

## Changing the crypto names and other options
In the `config` folder, the file `crypto_names.cpp` contains the tickers of the cryptos whose information is being fetched by the program. More crypto can be added, as long as they comply with the tickers included in the Bitstamp Api. It is possible to use also crypto included in other exchanges, of course, but then the `api.h` interface needs to be implemented by a new concrete class which adheres to the exhcange's Api standard. 

In the same folder, the `ohlc_params.json` file is used for the specification of the arguments in the Api request for the candlestick data (in this case, for the Bitstamp exchange: <https://www.bitstamp.net/api/#tag/Market-info/operation/GetOHLCData>). 

The system can also use files located in different locations, in which case the file paths must be specified when launching the programs (see the comments in the source code). 

## Output Examples
The `marketDataFetcher` program will present an output similar to the following: 

```
ADA/USD << percent_change_24: 2.59 << open_24: 0.38677 << bid: 0.39540 << side: 0 << ask: 0.39597 << vwap: 0.39538 << volume: 129109.63568150 << last: 0.39677 << high: 0.40413 << low: 0.38361 << open: 0.38842 << timestamp: 1720719943
--------------------------------------------------
BTC/USD << percent_change_24: 0.44 << open_24: 57590 << bid: 57841 << side: 0 << ask: 57850 << vwap: 58140 << volume: 2236.53575468 << last: 57844 << high: 59516 << low: 57072 << open: 57700 << timestamp: 1720719944
--------------------------------------------------
ETH/USD << percent_change_24: 0.96 << open_24: 3103.8 << bid: 3132.7 << side: 1 << ask: 3133.2 << vwap: 3138.5 << volume: 3302.20692798 << last: 3133.7 << high: 3213.6 << low: 3056.2 << open: 3100.8 << timestamp: 1720719943
--------------------------------------------------
SOL/USD << percent_change_24: -1.39 << open_24: 140.7629 << bid: 138.4351 << side: 1 << ask: 138.4887 << vwap: 141.3673 << volume: 21766.03 << last: 138.8028 << high: 145.9330 << low: 137.3858 << open: 141.7514 << timestamp: 1720719942
--------------------------------------------------
DOGE/USD << percent_change_24: -1.03 << open_24: 0.10926 << bid: 0.10803 << side: 0 << ask: 0.10812 << vwap: 0.10927 << volume: 2140471.25 << last: 0.10813 << high: 0.11209 << low: 0.10661 << open: 0.10799 << timestamp: 1720719943
```

`candlestickDataDownloader`, on the other hand, will download the candlestick data csv files in the `./data/` folder. An example of a csv file is as follows: 

```
DOGE/USD_volume,DOGE/USD_close,DOGE/USD_high,DOGE/USD_low,DOGE/USD_open,DOGE/USD_timestamp
70.00,0.07330,0.07330,0.07320,0.07320,2022-12-21 00:00:00
81135.49,0.07739,0.07739,0.07350,0.07350,2022-12-22 00:00:00
279016.31,0.07691,0.08500,0.07667,0.07880,2022-12-23 00:00:00
22294.37,0.07783,0.07794,0.07691,0.07691,2022-12-24 00:00:00
274873.01,0.07631,0.08800,0.07406,0.07751,2022-12-25 00:00:00
41847.94,0.07481,0.07624,0.07466,0.07603,2022-12-26 00:00:00
30510.23,0.07388,0.07595,0.07388,0.07595,2022-12-27 00:00:00
32372.24,0.07124,0.07262,0.07066,0.07262,2022-12-28 00:00:00
```

and the same information will be printed on screen in tabular format. The program `candlestickDataFetcher` will output the same information, but it will keep refreshing the data (by default, every 5 seconds). 

## Terminating the program
To terminate `marketDataFetcher` and `candlestickDataFetcher`, simply press `Ctrl+C`. `candlestickDataDownloader` terminates automatically. 
