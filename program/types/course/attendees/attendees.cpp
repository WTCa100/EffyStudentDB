#include "./attendees.hpp"

#include <sstream>

namespace Core::Types
{
    /// @brief This function appoints the minimal student & points pair.
    /// @note This idea was provided by Jakub Golab
    void Attendees::appointEdge()
    {
        for(const auto& [studentId, pair] : mappedStudents_)
        {
            if (pair.second < min_.second)
            {
                min_ = pair;
            }
        }
    }

    void Attendees::insertAttendee(std::shared_ptr<Student> newAttendee, const double& points)
    {
        std::pair<std::shared_ptr<Student>, double> studentPair = std::make_pair(newAttendee, points);
        mappedStudents_.insert(std::make_pair(newAttendee->id_, studentPair));
        if (min_.second > points || mappedStudents_.size() == 1)
        {
            min_ = studentPair;
        }
    }

    std::string Attendees::toString() const
    {
        std::stringstream ss;
        if (!mappedStudents_.empty())
        {
            ss << "Has " << mappedStudents_.size() << " attendees: \n";
            uint16_t count = 0;
            for (const auto& attendee : mappedStudents_)
            {
                ++count;
                double attendeePoints                   = attendee.second.second;
                std::shared_ptr<Student> currentStudent = attendee.second.first;
                ss << count << ". " << currentStudent->firstName_ << " "
                   << (currentStudent->secondName_.has_value() ? currentStudent->secondName_.value() + " " : " ")
                   << currentStudent->lastName_ << ": " << attendeePoints << " points";
                if (count != mappedStudents_.size()) ss << "\n";
            }
        }
        else { ss << "Has no attendees"; }
        return ss.str();
    }

    void Attendees::deleteAttendee(const uint16_t& id)
    {
        uint16_t currentMinId = min_.first->id_;
        mappedStudents_.erase(id);
        if (id == currentMinId)
        {
            appointEdge();
        }
    }

    Attendees::Attendees(size_t capacity) : capacity_(capacity), min_({nullptr, 0.0f}), mappedStudents_({})
    {
    }

} // namespace Core::Types
