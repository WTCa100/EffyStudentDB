#include "request.hpp"

#include <sstream>
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

}