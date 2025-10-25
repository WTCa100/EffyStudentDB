#include "./attendees.hpp"
#include "../../../utilities/common/constants.hpp"

#include <sstream>
#include <iostream>
namespace Core::Types
{
    /// @brief This function appoints the minimal student & points pair.
    /// @note This idea was provided by Jakub Golab
    void Attendees::appointEdge()
    {
        for(const auto& [studentId, pair] : mappedStudents_)
        {
            if (pair.second < min_.second || min_.first == nullptr)
            {
                min_ = pair;
            }
        }
    }

    /// @brief This function create and insert new pair into the mappedStudents_ ordered map.
    /// If new attendee has less points than the min_, then it will be come min_ only if the capacity is not reached.
    /// @param newAttendee shared pointer to a student object
    /// @param points points that given student got
    /// @return Information wether the capacity is full and previous min_ had to be dropped.
    Attendees::InsertionStatus Attendees::insertAttendee(std::shared_ptr<Student> newAttendee, const double& points)
    {        
        std::pair<std::shared_ptr<Student>, double> studentPair = std::make_pair(newAttendee, points);
        
        // Capacity reached and we need to drop the lowest rating student.
        if (min_.second < points && mappedStudents_.size() == capacity_)
        {
            mappedStudents_.insert(std::make_pair(newAttendee->id_, studentPair));
            mappedStudents_.erase(min_.first->id_);
            min_.first = nullptr;
            min_.second = Utilities::Common::Constants::maxPossibleScore;
            appointEdge();
            return InsertionStatus::addedMinimumChangedWithMaxCapacity;
        }

        // Capacity not reached, but this entry has less points than current min.
        if (min_.second > points || mappedStudents_.size() == 1)
        {
            mappedStudents_.insert(std::make_pair(newAttendee->id_, studentPair));
            min_ = studentPair;
            return InsertionStatus::addedMinimumChanged;
        }

        // Capacity not reached, and entry has more points than current min.
        if (mappedStudents_.size() < capacity_)
        {
            mappedStudents_.insert(std::make_pair(newAttendee->id_, studentPair));
            return InsertionStatus::added;
        }

        // Capacity is full, and student has less points then current minimal points ammount.
        // This is straight up means that this student is not eligable to join course.
        return InsertionStatus::notAddedPointsBelowMinValue;
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
