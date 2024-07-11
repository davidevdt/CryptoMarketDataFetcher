#pragma once

#include "json_reader.h"
#include <vector> 

/*
 * This class reads a string/file which contains a vector of Json objects, 
 * and it turns them into a vector of unordered map's.
 * Importantly, the input vector must be in the format [{}, {}, ..., {}] 
 * (this pattern can also be found as a substring in the original input). 
 */
class MultiJsonReader {
    
public:
    MultiJsonReader() {this->multiJsonObject.reserve(5);}

    MultiJsonReader(size_t n) {
        this->multiJsonObject.reserve(n);
    }

    MultiJsonReader(std::string inputString, bool isFile=false) {
        this->multiJsonObject.reserve(5); 
        if (!isFile) this->setFromString(inputString);
        else this->setFromFile(inputString); 
    }

    MultiJsonReader(size_t n, std::string inputString, bool isFile=false) {
        this->multiJsonObject.reserve(n); 
        if (!isFile) this->setFromString(inputString);
        else this->setFromFile(inputString); 
    }

    void setFromString(const std::string&); 
    void setFromFile(const std::string&); 
    auto get() const -> std::vector<jMap> {return multiJsonObject;}

    jMap& operator[](size_t i) {
        return multiJsonObject.at(i);  
    }

    friend std::ostream& operator<<(std::ostream& os, const MultiJsonReader& obj); 


private:
    JsonReader current; 
    std::vector<jMap> multiJsonObject; 
}; 