#include "action.hpp"

#include "../../../utilities/common/stringManip.hpp"

#include <iostream>

namespace Core
{
    std::string Action::getCommand() const
    {
        if (!tokens_.empty()) { return tokens_.at(static_cast<uint8_t>(ActionValue::command)); }
        return "";
    }

    std::string Action::getTarget() const
    {
        if (tokens_.size() >= 2) { return tokens_.at(static_cast<uint8_t>(ActionValue::target)); }
        return "";
    }

    std::vector<std::string> Action::getAdditionalValues() const
    {
        if (tokens_.size() >= 3)
        {
            std::vector<std::string> tags =
                std::vector<std::string>(tokens_.begin() + static_cast<uint8_t>(ActionValue::additionalValues), tokens_.end());
            return tags;
        }
        return {};
    }

}  // namespace Core
