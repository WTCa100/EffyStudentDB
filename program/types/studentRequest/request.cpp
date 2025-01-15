#include "request.hpp"

#include <iostream>
#include <sstream>

#include "../../utilities/inputHandler/inputHandler.hpp"
namespace Core::Types::Request
{
    std::string Srequest::toString() const
    {
        std::stringstream ss("");
        ss << "Request Id: " << id_ << " courseId: " << courseId_ << " studentId: " << studentId_ << " status: " << statusToString(status_);
        return ss.str();
    }

    std::string statusToString(const requestStatus& status)
    {
        switch (status)
        {
            case requestStatus::Approved:
                return "Approved";
            case requestStatus::Denied:
                return "Denied";
            case requestStatus::Pending:
                return "Pending";
            default:
                return "Unknown";
        }
    }

    std::map<std::string, std::string> Srequest::getAttrs() const
    {
        return {{"studentId", std::to_string(studentId_)}, {"courseId", std::to_string(courseId_)}, {"status", std::to_string(status_)}};
    }       

    void Srequest::userConstruct(bool makeFull)
    {
        std::cout << "Creating student request from user input\n";
        studentId_ = Utilities::InputHandler::getAttrAsNumber("Student ID");
        courseId_ = Utilities::InputHandler::getAttrAsNumber("Course ID");
        status_ = requestStatus::Pending;
    }

}