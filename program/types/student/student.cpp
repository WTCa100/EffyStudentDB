#include <iostream>
#include "student.hpp"

void showStudent(const Student& target)
{
    std::cout << "Student Display:\n";
    std::cout << "First Name: " << target.firstName_ << "\n";
    if(target.secondName_.has_value()) 
    {
        std::cout << "Second Name: " << target.secondName_.value() << "\n";
    }
    std::cout << "Last Name: " << target.lastName_ << "\n";

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