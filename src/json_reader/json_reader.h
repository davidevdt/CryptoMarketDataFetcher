#pragma once

#include <string> 
#include <unordered_map>
#include <fstream> 
#include <sstream>
#include <iostream> 
#include <vector> 
#include <algorithm>
#include "../utils/utils.cpp" 

using jMap = std::unordered_map<std::string,std::string>; 

/*
 * The class aims at converting a sequence of characters contained in a json object
 * (either as a string, or as an input file), parsing them, and converting them into 
 * an std::unordered_map<std::string,std::string> 
 */

class JsonReader {
    
public:
    // Constructors
    JsonReader() {}
    JsonReader(const std::string& inputString, bool isFile=false) {
        if (!isFile) this->setFromString(inputString);
        else this->setFromFile(inputString); 
    }

    // These methods parse the json object characters from a string or file, 
    // and convert them into std::unordered_map objects.
    void setFromString(const std::string&); 
    void setFromFile(const std::string&); 

    // Gets the parsed json objects
    auto get() const -> jMap {return jsonObject;}

    std::string& operator[](std::string key) {
        return jsonObject[key]; 
    }

    jMap operator[](std::vector<std::string> keys) {
        jMap filteredMap; 
        size_t itemsDone = 0; 
        for (const auto& item: jsonObject) {
            if (std::find(keys.begin(), keys.end(), item.first) != keys.end()) {
                filteredMap[item.first] = item.second; 
                if (++itemsDone == keys.size()) break; 
            }
        }
        return filteredMap;
    }

    friend std::ostream& operator<<(std::ostream& os, const JsonReader& obj); 


private:
    jMap jsonObject; 
}; 