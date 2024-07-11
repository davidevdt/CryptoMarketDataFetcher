#include "multi_json_reader.h"

// Reads a vector of json strings, and it turns them into a vector 
// of unordered maps. Note: the json vector is assumed to have form
// [{}, {}, {}, ..., {}] 
void MultiJsonReader::setFromString(const std::string& inputString) {
    if (inputString.size() == 0) return; 
    multiJsonObject.clear(); 

    size_t openSquareBracketPos = inputString.find("[{");
    if (openSquareBracketPos == std::string::npos) return; 
    size_t closeSquareBracketPos = inputString.find("}]");
    if (closeSquareBracketPos == std::string::npos) return; 
    auto dataList = inputString.substr(openSquareBracketPos+1, closeSquareBracketPos-openSquareBracketPos);

    std::stringstream ss{dataList};
    std::string tmpData; 
    while (std::getline(ss, tmpData, '}')) {
        size_t openBracket = tmpData.find(" {"); 
        if (openBracket != std::string::npos) tmpData = tmpData.substr(openBracket + 1); 
        JsonReader tmpJsonObject(tmpData + '}', false); 
        multiJsonObject.push_back(tmpJsonObject.get()); 
    } 
}

// Reads a vector of json objects from file, and it turns them into a vector 
// of unordered maps. To achieve this, it makes use of the setToString method
void MultiJsonReader::setFromFile(const std::string& inputFileName) {

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

std::ostream& operator<<(std::ostream& os, const MultiJsonReader& obj) {

    if (obj.multiJsonObject.empty()) return os; 

    std::stringstream out; 
    out << "[\n";
    for (const auto& item: obj.multiJsonObject) {
        out << Utils::mapToString(item, 2) << '\n'; 
    }
    out << ']';  
    os << out.str();
    return os; 
}