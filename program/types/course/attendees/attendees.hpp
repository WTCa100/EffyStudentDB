#pragma once

#include "../../../utilities/common/constants.hpp"
#include "../../student/student.hpp"

#include <inttypes.h>
#include <map>
#include <memory>

using Core::Types::Student;
typedef std::pair<std::shared_ptr<Student>, double> attendee;

namespace Core::Types
{

    class Attendees
    {
      public:
        enum class InsertionStatus
        {
            added                              = 0,
            addedMinimumChanged                = 1,
            addedMinimumChangedWithMaxCapacity = 2,
            notAddedPointsBelowMinValue        = 3
        };

      private:
        size_t capacity_;
        std::pair<std::shared_ptr<Student>, double> min_;
        std::map<uint16_t, attendee> mappedStudents_;
        void appointEdge();

      public:
        uint16_t getMinId() const { return min_.first->id_; }

        InsertionStatus insertAttendee(std::shared_ptr<Student> newAttendee, const double& points);
        void deleteAttendee(const uint16_t& id);
        std::string toString() const;

        inline std::map<uint16_t, attendee> getMappedStudentsCopy() { return mappedStudents_; }

        inline double getAttendeePoints(const uint16_t& id) { return mappedStudents_.at(id).second; }

        inline bool empty() const { return mappedStudents_.empty(); }

        inline size_t size() const { return mappedStudents_.size(); }

        Attendees():
            capacity_(1),
            min_({ nullptr, 0.0f }),
            mappedStudents_({})
        {}

        Attendees(size_t capacity);
        ~Attendees() = default;
    };
}  // namespace Core::Types
