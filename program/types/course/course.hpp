#pragma once

#include <inttypes.h>
#include <string>

struct Course
{
    uint16_t id_;
    uint16_t minStudents_;
    uint16_t maxStudents_;
    uint16_t baseMinimalPoints_;
    // Formula - on how to calculate points
    // One subject for now, range later (so, more than one subject can be applied to a given course)
    // Formula: [(1st subject grade * weight) + (2nd subject grade * weight) + (N-th subject grade * weight)] / N
    // Example: [(2 * math grade) + (1 * physics grade)] / 2
    std::string name_;
};
