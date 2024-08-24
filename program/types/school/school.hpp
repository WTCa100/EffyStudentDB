#pragma once

#include <string>
#include <vector>

#include "../student/student.hpp"

struct School
{
    uint16_t id_;
    std::string name_;
    std::vector<Student> students_;
};
