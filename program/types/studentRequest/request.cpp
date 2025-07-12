#include "request.hpp"

#include "../../utilities/inputHandler/inputHandler.hpp"

#include <iostream>
#include <sstream>

namespace Core::Types::Request
{
    std::string Srequest::toString() const
    {
        std::stringstream ss("");
        ss << "Request Id: " << id_ << " courseId: " << courseId_ << " studentId: " << studentId_
           << " status: " << statusToString(status_);
        return ss.str();
    }

    std::string statusToString(const requestStatus& status)
    {
        switch (status)
        {
            case requestStatus::Approved : return "Approved";
            case requestStatus::Denied : return "Denied";
            case requestStatus::Pending : return "Pending";
            default : return "Unknown";
        }
    }

    std::map<std::string, std::string> Srequest::getAttrs() const
    {
        // @TODO - Change all the magic numbers into global variables
        return {
            { "studentId",     std::to_string(studentId_) },
            { "courseId",      std::to_string(courseId_)  },
            { "requestStatus", std::to_string(status_)    }
        };
    }

    std::unordered_map<std::string, std::string> Srequest::userConstruct([[maybe_unused]] bool makeFull)
    {
        std::cout << "Creating student request from user input\n";
        std::unordered_map<std::string, std::string> attrs{};
        studentId_ = Utilities::InputHandler::getAttrAsNumberNonEmpty("Student ID");
        attrs.insert(std::make_pair("studentId", std::to_string(studentId_)));
        courseId_ = Utilities::InputHandler::getAttrAsNumberNonEmpty("Course ID");
        attrs.insert(std::make_pair("courseId", std::to_string(courseId_)));
        status_ = requestStatus::Pending;
        attrs.insert(std::make_pair("requestStatus", std::to_string(status_)));
        return attrs;
    }

    std::shared_ptr<Entry> Srequest::fillGaps(const std::shared_ptr<Entry> other)
    {
        std::shared_ptr<Srequest> concrete = std::static_pointer_cast<Srequest>(other);
        std::shared_ptr<Srequest> refObj   = std::make_shared<Srequest>();
        refObj->id_                        = id_ == 0 ? concrete->id_ : id_;
        refObj->studentId_                 = studentId_ == 0 ? concrete->studentId_ : id_;
        refObj->courseId_                  = courseId_ == 0 ? concrete->courseId_ : id_;
        refObj->status_                    = status_ == requestStatus::Unknown ? concrete->status_ : status_;
        return refObj;
    }

    Entry& Srequest::operator= (const Entry& other)
    {
        if (this == &other) return *this;

        const auto& otherSrequest = dynamic_cast<const Srequest&>(other);

        id_        = other.id_;
        studentId_ = otherSrequest.studentId_;
        courseId_  = otherSrequest.courseId_;
        status_    = otherSrequest.status_;

        return *this;
    }

}  // namespace Core::Types::Request