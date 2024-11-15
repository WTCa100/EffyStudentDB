#include "course.hpp"

#include <sstream>

namespace Core::Types
{
    std::string Course::toString() const
    {
        std::stringstream ss("");
        ss << "id: " << id_ << "name: " << name_ << " minimal student count: " << minStudents_ << " maximal student count: " << maxStudents_ << "\n"
           << "minimal points required: " << baseMinimalPoints_ << " average student points: " << averageStudentPoints_ << "\n";
        return ss.str();
    }
}