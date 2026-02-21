#include "PuzzleUtils.h"
#include <sstream>

// Parse a comma-separated pattern string 
// into a vector<bool> representing lever states.
std::vector<bool> parsePattern(const std::string& s) {
    std::vector<bool> out;
    std::stringstream ss(s);
    std::string token;

    while (std::getline(ss, token, ',')) {
        out.push_back(token == "1");
    }
    return out;
}

// Convert a vector<bool> back into a comma-separated pattern string. 
// This is used when saving or debugging lever states.
std::string patternToString(const std::vector<bool>& v) {
    std::string out;
    for (int i = 0; i < v.size(); i++) {
        out += (v[i] ? "1" : "0");
        if (i < v.size() - 1) out += ",";
    }
    return out;
}

// Convert a raw pattern string into a human-readable form.
std::string formatPattern(const std::string& pattern) {
    std::stringstream ss(pattern);
    std::string token;
    std::string out;

    while (std::getline(ss, token, ',')) {
        out += (token == "1" ? "UP" : "DOWN");
        if (!ss.eof()) out += " / ";
    }
    return out;
}
