#pragma once

#include <string>
#include <vector>

namespace Utilities::Common
{
    std::vector<std::string> tokenize(std::string rawFormat, const char delim);
    std::string assemble(std::vector<std::string> tokens, const char delim);
}  // namespace Utilities::Common
