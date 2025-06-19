#pragma once

#include <string>
#include <vector>

namespace Utilities::Common
{
    std::vector<std::string> tokenize(std::string rawFormat, const char delim);
}  // namespace Utilities::Common
