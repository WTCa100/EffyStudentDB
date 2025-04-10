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

    std::unordered_map<std::string, std::string> Srequest::userConstruct(bool makeFull)
    {
        std::cout << "Creating student request from user input\n";
        studentId_ = Utilities::InputHandler::getAttrAsNumberNonEmpty("Student ID");
        courseId_ = Utilities::InputHandler::getAttrAsNumberNonEmpty("Course ID");
        status_ = requestStatus::Pending;
        return {};
    }

    Entry& Srequest::operator=(const Entry& other) 
    {
        if(this == &other) return *this;

        const auto& otherSrequest = dynamic_cast<const Srequest&>(other);
        
        id_        = other.id_;
        studentId_ = otherSrequest.studentId_;
        courseId_  = otherSrequest.courseId_;
        status_    = otherSrequest.status_;

        return *this;
    }


}