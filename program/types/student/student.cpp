#include <iostream>
#include "student.hpp"

namespace Core::Types
{
    void showStudent(const Student& target, bool showGrades)
    {
        std::cout << "First Name: " << target.firstName_ << ", ";
        if(target.secondName_.has_value()) 
        {
            std::cout << "Second Name: " << target.secondName_.value() << ", ";
        }
        std::cout << "Last Name: " << target.lastName_ << ", ";
        std::cout << "Email: " << target.email_; 
        if(showGrades)
        {
            std::cout << ", ";
            if(!target.grades_.empty())
            {
                std::cout << "Grades: \n";
                for(const auto& g : target.grades_)
                {
                    std::cout << g.first << " : " << g.second << "\n";
                }
            }
            else
            {
                std::cout << "No Grades!\n";
            }
        }
    }
} // namespace Core::Types
