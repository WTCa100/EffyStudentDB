#pragma once

#include <vector>
#include <string>

#include "../../../utilities/common/stringManip.hpp"

namespace Core
{

    // TODO suggestion - check valid commands?

    enum class ActionValue
    {
        command        = 0,
        target         = 1,
        optionalValues = 2
    };

    struct Action
    {
        std::vector<std::string> tokens_;

        Action() : tokens_({}) {}
        Action(const std::string& input, const char& delim = ' ') : tokens_(Utilities::Common::tokenize(input, delim)) {}
        ~Action() = default;
        std::string getCommand() const;
        std::string getTarget() const;
        std::vector<std::string> getOptValues() const;

    };
    
} // namespace Core


