#include "stringManip.hpp"

#include <iostream>
#include <sstream>

namespace Utilities::Common
{
    std::vector<std::string> tokenize(std::string rawFormat, const char delim)
    {
        std::vector<std::string> tokens;
        std::string token;
        std::istringstream iss(rawFormat);

        while (std::getline(iss, token, delim)) { tokens.push_back(token); }
        return tokens;
    }

    std::string assemble(std::vector<std::string> tokens, const char delim)
    {
        std::stringstream result("");
        for (std::string& word : tokens) { result << word << delim; }
        return result.str();
    }
}  // namespace Utilities::Common
