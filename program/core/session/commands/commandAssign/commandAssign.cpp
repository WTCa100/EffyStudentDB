#include "commandAssign.hpp"

namespace Core::Commands
{
    bool CommandAssign::exec()
    {
        LOG((*logger_), "Executing CommandAssign for courseId = ", targetCourseId_, " studentId = ", targetStudentId_);

        if (sAdapter_->addAttendee(targetStudentId_, targetCourseId_))
        {
            LOG((*logger_), "Successfully added new assignee to course.");
            return true;
        }
        LOG((*logger_), "Could not add assignee to course.");
        return false;
    }
}  // namespace Core::Commands
