#include "displayHelper.hpp"

#include "../../types/course/attendees/attendees.hpp"
#include "../../utilities/common/constants.hpp"

namespace Core::Display
{

    using Core::Types::Attendees;
    using Utilities::Common::Constants::AttendeeValuePosition;

    void Helper::displayAttendedCourses(const uint16_t& studentId) const
    {
        std::string filter                                       = "studentId = " + std::to_string(studentId);
        std::vector<Utilities::Sql::rawAttendee> courseAttendees = sAdapter_->getAttendees(filter);
        if (courseAttendees.empty())
        {
            std::cout << "Not attending any courses.\n";
            return;
        }

        std::shared_ptr<abstractTypeList> dataCourses = data_->getEntries(g_tableCourses);
        for (const auto& attendee : courseAttendees)
        {
            uint16_t courseId   = std::get<static_cast<uint16_t>(AttendeeValuePosition::courseId)>(attendee);
            double studentScore = std::get<static_cast<uint16_t>(AttendeeValuePosition::points)>(attendee);
            if (dataCourses->contains(courseId))
            {
                std::shared_ptr<Course> currentCourse = std::dynamic_pointer_cast<Course>(dataCourses->at(courseId));
                std::cout << currentCourse->id_ << ". " << currentCourse->name_ << " Points = " << studentScore << "\n";
            }
            else
            {
                LOG((*logger_), "No such course in session: ", courseId, " found in session data.");
                std::cout << courseId << ". ERROR: No such course found in Session data!\n";
            }
        }
    }

    void Helper::displayAttendees(const uint16_t& courseId) const
    {
        const std::string filter                                       = "courseId = " + std::to_string(courseId);
        const std::vector<Utilities::Sql::rawAttendee> courseAttendees = sAdapter_->getAttendees(filter);
        if (courseAttendees.empty())
        {
            std::cout << "Has no attendees.\n";
            return;
        }

        const std::shared_ptr<abstractTypeList> dataStudents = data_->getEntries(g_tableStudents);
        std::cout << "Attendee count: " << courseAttendees.size() << " students." << std::endl;
        for (const auto& attendee : courseAttendees)
        {
            const uint16_t studentId  = std::get<static_cast<uint16_t>(AttendeeValuePosition::studentId)>(attendee);
            const double studentScore = std::get<static_cast<uint16_t>(AttendeeValuePosition::points)>(attendee);
            if (dataStudents->contains(studentId))
            {
                const std::shared_ptr<Student> currentStudent = std::dynamic_pointer_cast<Student>(dataStudents->at(studentId));
                std::cout << currentStudent->id_ << ". " << currentStudent->firstName_
                          << (currentStudent->secondName_ ? (" " + currentStudent->secondName_.value() + " ") : " ")
                          << currentStudent->lastName_ << " Points: " << studentScore << "\n";
            }
            else
            {
                LOG((*logger_), "No such student in session: ", studentId, " found in session data.");
                std::cout << studentId << ". ERROR: No such student found in Session data!\n";
            }
        }
    }

    void Helper::displayGrades(const uint16_t& studentId) const
    {
        const std::string filter                             = "studentId = " + std::to_string(studentId);
        const std::vector<Grade> grades                      = sAdapter_->getGrades(filter);
        const std::shared_ptr<abstractTypeList> dataSubjects = data_->getEntries(g_tableSubjects);
        for (const auto& grade : grades)
        {
            std::cout << grade.id_ << ". ";
            const uint16_t& targetSubjectId = grade.subjectId_;
            if (dataSubjects->contains(targetSubjectId))
            {
                const std::shared_ptr<Subject> currentSubject =
                    std::dynamic_pointer_cast<Subject>(dataSubjects->at(targetSubjectId));
                std::cout << currentSubject->name_ << ": " << grade.value_ << "\n";
            }
            else
            {
                LOG((*logger_), "No such subject in session: ", grade.subjectId_,
                    " found in session data. Internal subject name = ", grade.subjectName_);
                std::cout << grade.subjectId_
                          << " ERROR: No such subject found in Session data! (Internal info only: grade.value_ = " << grade.value_
                          << " grade.subjectName_ " << grade.subjectName_ << ")";
            }
        }
    }

    Helper::Helper(
        std::shared_ptr<Logger> logger, std::shared_ptr<SqlAdapter> sqlAdapter, std::shared_ptr<SessionData> sessionData):
        logger_(logger),
        sAdapter_(sqlAdapter),
        data_(sessionData)
    {
        LOG((*logger_), "Dispaly Helper object created.");
    }

}  // namespace Core::Display
