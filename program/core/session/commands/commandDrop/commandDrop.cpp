#include "commandDrop.hpp"

namespace Core::Commands
{
    bool CommandDrop::exec()
    {
        LOG((*logger_), "Executing CommandDrop for courseId = ", targetCourseId_, " studentId = ", targetStudentId_);

        if (sAdapter_->removeAttendee(targetStudentId_, targetCourseId_))
        {
            LOG((*logger_), "Successfully dropped assignee from course.");
            return true;
        }
        LOG((*logger_), "Could not drop assignee from course.");
        return false;
    }
}  // namespace Core::Commands
