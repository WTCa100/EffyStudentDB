#include <iostream>
#include <sstream>
#include "student.hpp"

namespace Core::Types
{
    std::string Student::toString(bool showGrades) const
    {
        std::stringstream ss("");
        ss << "Id: " << id_ << ", First name: " << firstName_ << ", ";
        if(secondName_.has_value())
        {
            ss << "Second name: " << secondName_.value() << ", ";
        }
        ss << "Last name: " << lastName_ << ", ";
        ss << "Email: " << email_ << " ";
        if(showGrades)
        {
            if(!grades_.empty())
            {
                ss << ", Grades: \n";
                for(const auto& grade : grades_)
                {
                    ss << grade.first << ":" << grade.second << "\n";
                }
            }
            else
            {
                ss << "No grades.";
            }
        }
        return ss.str();
    }
} // namespace Core::Types
