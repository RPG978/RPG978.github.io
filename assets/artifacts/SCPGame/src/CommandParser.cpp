#include "CommandParser.h"
#include <algorithm>
#include <sstream>

CommandParser::Command CommandParser::parse(const std::string& input) const {
    Command cmd;

    // Make a lowercase copy of the input
    std::string normalized = input;
    std::transform(
        // start of input range
        normalized.begin(), 
        // end of input range
        normalized.end(),
        // start of output range
        normalized.begin(),
        // function to apply to each element
        [](unsigned char c) { 
            return std::tolower(c); 
        }
    );

    // Trim leading/trailing whitespace
    auto isSpace = [](unsigned char c) { 
        return std::isspace(c); 
        };

    // left trim
    normalized.erase(
        normalized.begin(),
        std::find_if(
            normalized.begin(), 
            normalized.end(),
            [&](char c) { 
                return !isSpace(c); 
            }
        )
    );

    // right trim
    normalized.erase(
        // finds the last non-space
        std::find_if(
            normalized.rbegin(), 
            normalized.rend(),
            [&](char c) { 
                return !isSpace(c); 
            }
        ).base(), // gives a forward iterator after search
        normalized.end()
    );

    // Split into verb + argument
    std::istringstream iss(normalized);
    iss >> cmd.verb;          // first word
    std::getline(iss, cmd.argument); // remainder

    // Trim leading space from argument if present
    if (!cmd.argument.empty() && cmd.argument[0] == ' ')
        cmd.argument.erase(0, 1);

    return cmd;
}
