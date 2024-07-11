#pragma once 

#include <string> 
#include <cstdlib> 
#include <unordered_map> 
#include <vector> 
#include <sstream> 
#include <algorithm> 
#include <ctime> 
#include <iomanip> 
#include <iostream> 
#include <fstream> 

/*
 * Class containing a set of utility methods.
 */

class Utils {

public:
    // Returns 0 if the directory already existed 
    static inline int createNewFolder(std::string folderName); 

    // Turns an std::unordered_map<std::string,std::string> into a std::string format
    // that can be printed to screen. 
    static inline std::string mapToString(const std::unordered_map<std::string, std::string>& map, int depth=1);

    // Turns a vector of std::unordered_map<std::string,std::string> into a std::string format
    // that can be printed to screen. 
    static inline std::string mapVectorToString(const std::vector<std::unordered_map<std::string, std::string>>& mapVector); 

    /* If formats a vector of std::unordered_map<std::string,std::string> objects.
     * The output can be either in tabular format (toCsv=false), or in csv format (toCsv=true). 
     * The headerPrefix argument adds a prefix to the header names; if timestampName is specified, 
     * the field corresponding to that name is converted into datetime format; exclude is a vector
     * containing the name of the fields to discard in the formatted output.  
    */   
    static inline std::string formatMapVector(
        const std::vector<std::unordered_map<std::string, std::string>>& mapVector, 
        const std::string& headerPrefix = "",
        const std::string& timestampName = "", 
        const std::vector<std::string>& exclude = {}, 
        bool toCsv = true
    ); 

    // Turns a timestamp integer (taken as input) and returns the corresponding 
    // datetime object, into string format. 
    static inline std::string timestampToString(int timestamp, bool toUtc=true); 

    // It takes a datetime string as an input, and it returns the corresponding 
    // timestamp as an integer. 
    static inline int stringToTimestamp(const std::string& dateTimeStr, bool fromUtc=true); 

    // Converts the values (and keys) of a unordered_map into messages, e.g. for print-screen updates
    static inline std::stringstream mapToMessage(
        const std::string& name, 
        const std::unordered_map<std::string, std::string>& map,
        const std::vector<std::string>& fields = {}
    );  

    /* Converts a "matrix" (vector of vectors of std::string's) into a 
     * message that can be screen-printed; each column will corresponds to 
     * the outer vectors. The first column is always a timestamp, whose n must be provided 
     * as an input.
     */ 
    static inline std::stringstream matrixToMsg(
        const std::vector<std::string>& names, 
        const std::vector<std::string>& timestampVector, 
        const std::vector<std::vector<std::string>>& values
    );

    // It takes a string and a filename as input, and writes 
    // the string into the given file. 
    static inline int writeStringToFile(
        const std::string& content, 
        const std::string& fileName
    ); 

    // It takes a filename as an input and, if found, it stores its lines into a 
    // std::vector. Throws a runtime error if it could not read the file. 
    static inline std::vector<std::string> readTxtLines(const std::string& fileName);

private: 

    // Turns a vector of unordered_map's into a vector of string vectors (matrix), 
    // where each of the map's keys corresponds to a column. 
    static inline std::vector<std::vector<std::string>>  mapVectorToMatrix(
        const std::vector<std::unordered_map<std::string, std::string>>& mapVector,
        std::vector<std::string>& header, 
        int& timestampPos, 
        const std::string& timestampName
    ); 

    // Converts a vector of string vectors (matrix) into a tabular format. 
    static inline std::string matrixToTableFormat(
        const std::vector<std::vector<std::string>>& mapData, 
        const std::vector<std::string>& header, 
        const std::vector<std::string>& exclude, 
        const std::string& headerPrefix, 
        int timestampPos, 
        int colWidth = 15
    ); 

    // It turns a vector of string vectors into a csv format. 
    static inline std::string matrixToCsvFormat(
            const std::vector<std::vector<std::string>>& mapData, 
        const std::vector<std::string>& header, 
        const std::vector<std::string>& exclude, 
        const std::string& headerPrefix, 
        int timestampPos
    );
}; 