#include <string> 
#include <sstream>
#include <iostream>
#include <cstdio>

/*
 * The class makes use of `curl` to perform web requests. The 'request' method performs the actual web requests,
 * given an url as input. The 'setMaxConnectionTime' method sets the max waiting time for a request.
 */

class HttpRequest {

public:
    // Constructors; at initialization, the base curl command for terminal is created 
    HttpRequest() {this->buildCommand();} 
    HttpRequest(std::string agentHeader): userAgentHeader(agentHeader) {this->buildCommand();} 
    HttpRequest(int connectionTime): maxConnectionTime(connectionTime) {this->buildCommand();} 
    HttpRequest(std::string agentHeader, int connectionTime): userAgentHeader(agentHeader), maxConnectionTime(connectionTime) {
        this->buildCommand();
    } 

    std::string getUserAgentHeader() const {return userAgentHeader;} 
    int getMaxConnectionTime() const {return maxConnectionTime;} 

    void setUserAgentHeader(std::string newAgentHeader) {userAgentHeader = newAgentHeader; this->buildCommand();}
    void setMaxConnectionTime(int newMaxConnectionTime) {
        if (newMaxConnectionTime < - 1) newMaxConnectionTime = -1; 
        maxConnectionTime = newMaxConnectionTime; 
        this->buildCommand(); 
    }
    
    std::string getCommand() {
        return cmd; 
    }

    // Performs the web request 
    std::string request(const std::string& url) const {
        return exec((cmd + "\"" + url + "\"").c_str()); 
    } 


private:
    std::string cmd; 
    std::string userAgentHeader = "User-Agent: Mozilla/5.0"; 
    int maxConnectionTime = 100; // max connection time 

    // Creates the base curl command for the web request (without url) 
    void buildCommand() {
        if (cmd.size() != 0) cmd.clear(); 
        cmd = "curl -s -H \"" + userAgentHeader + "\" "; 
        if (maxConnectionTime != -1) {
            cmd += "--max-time " + std::to_string(maxConnectionTime) + " "; 
        }
    }

    // Executes a terminal command 
    static std::string exec(const char* cmd_) {
        char buffer[128];
        std::string result = "";
        FILE* pipe = popen(cmd_, "r");
        if (!pipe) {
            return "popen failed!";
        }
        while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            result += buffer;
        }
        pclose(pipe);
        return result;
    }

};