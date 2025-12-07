#include "commandOpen.hpp"

namespace Core::Commands
{
    bool CommandOpen::exec()
    {
        LOG((*logger_), "Executing CommandOpen for ");
        if (sAdapter_->openCourse(targetCourseId_))
        {
            LOG((*logger_), "Course with id =", targetCourseId_, " has been opened!");
            std::shared_ptr<Course> targetCourse = std::static_pointer_cast<Course>(sessionData_->getEntry(targetCourseId_, g_tableCourses));
            targetCourse->isOpen_ = Utilities::Common::Constants::OpenState::opened;
            std::cout << "Opened course!\n";
            return true;
        }

        LOG((*logger_), "Failed opening course with id=", targetCourseId_);
        std::cout << "Could not open course!\n";
        return false;
    }
    
} // namespace Core::Command
