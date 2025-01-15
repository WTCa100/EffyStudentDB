#include <iostream>
#include <sstream>
#include "student.hpp"

#include "../../utilities/inputHandler/inputHandler.hpp"

namespace Core::Types
{
    std::string Student::toString(bool showGrades) const
    {
        std::stringstream ss("");
        ss << toString() << "\n";
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

    std::string Student::toString() const
    {
        std::stringstream ss("");
        ss << "Id: " << id_ << ", First name: " << firstName_ << ", ";
        if(secondName_.has_value())
        {
            ss << "Second name: " << secondName_.value() << ", ";
        }
        ss << "Last name: " << lastName_ << ", ";
        ss << "Email: " << email_ << " ";
        return ss.str();
    }

    std::map<std::string, std::string> Student::getAttrs() const
    {
        std::map<std::string, std::string> mappedAttrs;
        mappedAttrs.insert(std::make_pair("firstName", firstName_));
        if(secondName_.has_value()) mappedAttrs.insert(std::make_pair("secondName", secondName_.value())); 
        mappedAttrs.insert(std::make_pair("lastName", lastName_));
        mappedAttrs.insert(std::make_pair("email", email_));
        mappedAttrs.insert(std::make_pair("schoolId", std::to_string(schoolId_)));
        return mappedAttrs;
    }

    void Student::userConstruct(bool makeFull)
    {
        std::cout << "Creating student from user input\n";
        std::cout << "(Attributes tagged as 'optional' can be left empty)\n";
        firstName_ = Utilities::InputHandler::getAttrAsStringNonEmpty("First name");
        secondName_ = Utilities::InputHandler::getAttrAsString("Second name (optional)");
        lastName_ = Utilities::InputHandler::getAttrAsStringNonEmpty("Last name");
        email_ = Utilities::InputHandler::getAttrAsStringNonEmpty("Email");
        schoolId_ = Utilities::InputHandler::getAttrAsNumber("Associated School (ID)"); // Maybe turn it into optional?
        return;
    }

    Student::Student(uint16_t id, std::string name, std::string lastName, std::string email, uint16_t schoolId, std::optional<std::string> secondName) : 
             Entry(id_, "Schools"),
             firstName_(name),
             secondName_(secondName),
             lastName_(lastName),
             email_(email),
             grades_({}),
             schoolId_(schoolId)
    {}

    Student::Student(std::string name, std::string lastName, std::string email, uint16_t schoolId, std::optional<std::string> secondName) :
             Entry(0, "Schools"),
             firstName_(name),
             secondName_(secondName),
             lastName_(lastName),
             email_(email),
             grades_({}),
             schoolId_(schoolId)
    {}

} // namespace Core::Types
