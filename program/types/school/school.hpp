#pragma once

#include <string>
#include <map>

#include "../student/student.hpp"

namespace Core::Types
{
    struct School
    {
        uint16_t id_;
        std::string name_;
        std::map<uint16_t, const Student*> students_;
    };
} // namespace Core

