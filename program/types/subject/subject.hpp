#pragma once

#include <string>

namespace Core::Types
{
    struct Subject
    {
        uint16_t    id_;
        std::string name_;
        inline std::string toString() const { return std::to_string(id_) + ". " + name_; }
    };

} // namespace Core::Types
