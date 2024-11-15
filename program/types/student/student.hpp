#pragma once

#include <map>
#include <string>
#include <optional>
#include <memory>

#include "../subject/subject.hpp"

namespace Core::Types
{
    struct Student
    {
        uint16_t id_;
        std::string firstName_;
        std::optional<std::string> secondName_;
        std::string lastName_;
        std::string email_;
        std::map<std::string, float> grades_;
        uint16_t schoolId_;
        std::string toString(bool showGrades = false) const;
    };
} // namespace Core::Types
