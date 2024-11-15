#pragma once

#include <string>
#include <vector>

#include "../student/student.hpp"

namespace Core::Types
{
    struct School
    {
        uint16_t id_;
        std::string name_;
        std::vector<Student> students_;
        std::string toString() const;
    };
} // namespace Core

