#pragma once

#include <map>
#include <string>
#include <optional>
#include <memory>

#include "../subject/subject.hpp"

struct Student
{
    std::string firstName_;
    std::optional<std::string> secondName_;
    std::string lastName_;
    std::map<subject, float> grades_;
};

void showStudent(const Student& target);