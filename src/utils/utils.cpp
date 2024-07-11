#include "utils.h" 
#include <algorithm>
#include <ctime>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>

int Utils::createNewFolder(std::string folderName) {
    std::string cmdCheck = "[ -d \"" + folderName + "\" ]";
    int check = std::system(cmdCheck.c_str());

    if (check == 0) return check;

    std::string cmdCreate = "mkdir " + folderName;
    std::system(cmdCreate.c_str()); 
    return 1; 
}

std::string Utils::mapToString(const std::unordered_map<std::string, std::string>& map, int depth) {
    if (depth <= 0) depth = 1;

    std::ostringstream out;
    std::string tabBaseDepth(depth - 1, ' ');
    std::string tabDepth(depth, ' ');

    out << tabBaseDepth << "{\n";
    for (const auto& item : map) {
        out << tabDepth << item.first << ": " << item.second << "\n";
    }
    out << tabBaseDepth << '}';

    return out.str();
}

std::string Utils::mapVectorToString(const std::vector<std::unordered_map<std::string, std::string>>& mapVector) {
    std::ostringstream out;
    out << "[\n";
    for (const auto& item : mapVector) {
        out << mapToString(item, 2) << '\n';
    }
    out << ']';
    
    return out.str();
}

std::string Utils::formatMapVector(
    const std::vector<std::unordered_map<std::string, std::string>>& mapVector, 
    const std::string& headerPrefix,
    const std::string& timestampName, 
    const std::vector<std::string>& exclude, 
    bool toCsv
) {
    std::vector<std::string> header; 
    int timestampPos = -1; 
    auto mapData = mapVectorToMatrix(mapVector, header, timestampPos, timestampName); 

    return toCsv ? 
        matrixToCsvFormat(mapData, header, exclude, headerPrefix, timestampPos) :
        matrixToTableFormat(mapData, header, exclude, headerPrefix, timestampPos); 
         
}

std::string Utils::timestampToString(int timestamp, bool toUtc) {
    // Convert the integer to a time_t type
    time_t rawTime = static_cast<time_t>(timestamp);

    // Convert the time_t to a tm structure in UTC
    struct tm *timeInfo = toUtc? gmtime(&rawTime) : localtime(&rawTime);

    // Check if the conversion was successful
    if (timeInfo == nullptr) {
        throw std::runtime_error("Error converting timestamp to UTC time.");
    }

    // Format the date and time into a readable string
    char buffer[20];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeInfo);

    // Return the formatted string
    return std::string(buffer);
}

int Utils::stringToTimestamp(const std::string& dateTimeStr, bool fromUtc) {
    std::tm timeInfo = {};
    std::istringstream ss(dateTimeStr);
    ss >> std::get_time(&timeInfo, "%Y-%m-%d %H:%M:%S");

    if (ss.fail()) {
        throw std::runtime_error("Failed to parse date and time string.");
    }

    // Convert to time_t in local time
    int timestamp = fromUtc ? static_cast<int>(timegm(&timeInfo)) : static_cast<int>(std::mktime(&timeInfo)); 
    return timestamp;
}

std::stringstream Utils::mapToMessage(
    const std::string& name, 
    const std::unordered_map<std::string, std::string>& map,
    const std::vector<std::string>& fields) {

    std::stringstream msg; 
    msg << name << " << "; 

    if (fields.empty()) {
        for (auto it = map.begin(); it != map.end(); ++it) {
            msg << it->first << ": " << it->second; 
            if (std::next(it) != map.end()) msg << " << "; 
        }
    } else {
        for (size_t i = 0; i < fields.size(); ++i) {
            auto it = map.find(fields.at(i)); 
            if (it != map.end()) {
                msg << it->first << ": " << it->second; 
                if (i < fields.size()-1) msg << " << "; 
            }
        }
    }
    return msg; 
}

int Utils::writeStringToFile(
    const std::string& content, 
    const std::string& fileName
) {
    std::fstream outFile(fileName, std::ios::out);

    if (!outFile) {
        std::cerr << "Cannot write to file \"" << fileName << "\" (please check that the directory exists)." << std::endl; 
        return 0; 
    } 

    outFile << content << std::endl; 
    return 1; 
}   

std::stringstream Utils::matrixToMsg(
    const std::vector<std::string>& names, 
    const std::vector<std::string>& timestampVector, 
    const std::vector<std::vector<std::string>>& values
) {

        std::stringstream msg; 

        const int timestampWidth = 25;
        const int valueWidth = 15;

        // Header
        msg << std::left << std::setw(timestampWidth) << "Timestamp";
        for (const auto& n: names) msg << std::left << std::setw(valueWidth) << n; 
        msg << "\n"; 
        msg << std::string(timestampWidth + valueWidth * names.size(), '-') << "\n"; 

        // Values 
        for (size_t i = 0; i < timestampVector.size(); ++i) {
            msg << std::left << std::setw(timestampWidth) << timestampVector.at(i); 
            for (size_t j = 0; j < values.size(); ++j) {
                msg << std::left << std::setw(valueWidth) << values.at(j).at(i); 
            }
            msg << "\n"; 
        }
        msg << std::string(timestampWidth + valueWidth * names.size(), '-') << "\n"; 
        return msg; 
}

std::vector<std::string> Utils::readTxtLines(const std::string& fileName) {
    std::fstream inFile(fileName, std::ios::in); 

    if (!inFile) {
        throw(std::runtime_error("Failed to read file: " + fileName)); 
    }

    std::vector<std::string> lines; 
    std::string line; 
    while (std::getline(inFile, line)) {
        if (line == "" || line == " " || line == "\t" || line == "\n") continue;
        if (line.back() == ' ' || line.back() == '\n') line.pop_back(); 
        lines.push_back(line); 
    } 
    inFile.close(); 
    return lines;  
}

/************************
*   Private Functions   * 
*************************/ 
std::vector<std::vector<std::string>> Utils::mapVectorToMatrix(
    const std::vector<std::unordered_map<std::string, std::string>>& mapVector,
    std::vector<std::string>& header, 
    int& timestampPos, 
    const std::string& timestampName
) {
    std::vector<std::vector<std::string>> mapData; 

    // Create a "data matrix" (vector of vectors) with the data
    for (size_t i = 0; i < mapVector.size(); ++i) {
        std::vector<std::string> tmp(mapVector.at(i).size()); 
        for (const auto& item: mapVector.at(i)) {
            if (i == 0) header.push_back(item.first); 
            size_t vecPos = std::distance(
                header.begin(), 
                std::find(header.begin(), header.end(), item.first)
            );
            if (timestampName != "" && std::find(header.begin(), header.end(), timestampName) != header.end()) {
                timestampPos = vecPos; 
            }
            tmp.at(vecPos) = item.second;  
        }
        mapData.push_back(tmp); 
    }
    return mapData; 
}

std::string Utils::matrixToTableFormat(
    const std::vector<std::vector<std::string>>& mapData, 
    const std::vector<std::string>& header, 
    const std::vector<std::string>& exclude, 
    const std::string& headerPrefix, 
    int timestampPos, 
    int colWidth
) {

    std::stringstream out;
    colWidth += headerPrefix.size();  
    for (size_t i = 0; i < header.size(); ++i) {
        if (std::find(exclude.begin(), exclude.end(), header.at(i)) != exclude.end()) {continue;} 
        out << std::left << std::setw(colWidth) << headerPrefix + header.at(i); 
    } 
    out << std::endl; 

    out << std::string(header.size() * colWidth, '-') << std::endl; 

    for (const auto& row: mapData) {
        size_t j = 0; 
        for (const auto& col: row) {
            if (timestampPos >= 0 && j == timestampPos) {
                out << std::left << std::setw(colWidth) << timestampToString(std::stoi(col));
                continue; 
            }
            if (std::find(exclude.begin(), exclude.end(), header.at(j)) != exclude.end()) {++j; continue;} 
            out << std::left << std::setw(colWidth) << col;
            ++j; 
        }
        out << std::endl; 
    }
    out << std::string(header.size() * colWidth, '-') << std::endl; 
    
    return out.str(); 
}

std::string Utils::matrixToCsvFormat(
    const std::vector<std::vector<std::string>>& mapData, 
    const std::vector<std::string>& header, 
    const std::vector<std::string>& exclude, 
    const std::string& headerPrefix, 
    int timestampPos
) {
    std::stringstream out;
    for (size_t i = 0; i < header.size(); ++i) {
        if (std::find(exclude.begin(), exclude.end(), header.at(i)) != exclude.end()) {continue;} 
        out << headerPrefix + header.at(i); 
        if (i < header.size() - 1) out << ","; 
    } 
    out << std::endl; 

    for (const auto& row: mapData) {
        size_t j = 0; 
        for (const auto& col: row) {
            if (timestampPos >= 0 && j == timestampPos) {
                out << timestampToString(std::stoi(col));
                if (j < row.size()-1) out << ","; 
                continue; 
            }
            if (std::find(exclude.begin(), exclude.end(), header.at(j)) != exclude.end()) {++j; continue;} 
            out << col;
            if (j < row.size() - 1) out << ","; 
            ++j; 
        }
        out << std::endl; 
    }

    return out.str(); 
}