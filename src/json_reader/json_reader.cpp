#include "json_reader.h"
#include <fstream>

// It reads a json object (passed as a string) as an input, and it stores it into a 
// unordered map object, where both the key and the value are stored
// as std::string's. 
void JsonReader::setFromString(const std::string& inputString) {

    if (inputString.size() == 0) return; 

    this->jsonObject.clear(); 
    std::stringstream ss{inputString}; 
    std::string key{}; 
    std::string value{}; 
    bool doValue = false;
    bool isList = false;  
    char currentChar; 

    char skip = ss.peek(); 
    if (skip == '{' || skip == '"') ss.ignore(); 
    skip = ss.peek(); 
    if (skip == '"') ss.ignore(); 

    while (ss.get(currentChar)) {
        if (currentChar == '"' || currentChar == ' ') {
            continue;
        }

        if (currentChar == ':') {
            doValue = true;
            continue;
        }

        if (currentChar == '[' && !isList) isList = true; 
        if (currentChar == ']' && isList) isList = false;
        
        if (currentChar != '}' && (currentChar != ',' || (currentChar == ',' && isList))) {
            if (!doValue) key += currentChar; 
            else value += currentChar;
            continue; 
        }

        if (currentChar == ',' || currentChar == '}') {
            this->operator[](key) = value; 
            key.clear(); 
            value.clear(); 
            doValue = false; 
            ss.ignore(); 
        }
    }
}

// It creates an unordered map from a json file. It makes use
// of the setFromString method. 
void JsonReader::setFromFile(const std::string& inputFileName) {

    std::fstream inputFile(inputFileName, std::ios::in); 

    if (!inputFile.is_open()) {
        std::cerr << "Error: Could not open the file" << inputFileName << std::endl; 
        return; 
    }

    std::stringstream stringFileStream; 
    std::string jsonLine;  
    while (std::getline(inputFile, jsonLine)) stringFileStream << jsonLine; 
    setFromString(stringFileStream.str()); 
    inputFile.close(); 
}

std::ostream& operator<<(std::ostream& os, const JsonReader& obj) {
    if (obj.jsonObject.empty()) return os; 
    os << Utils::mapToString(obj.jsonObject);
    return os; 
}