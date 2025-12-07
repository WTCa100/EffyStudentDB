#include "commandFind.hpp"

namespace Core::Commands
{
    bool CommandFind::exec()
    {
        LOG((*logger_), "Executing CommandFind");
        if (filterInput_.empty())
        {
            LOG((*logger_), "No values in Filter Input!");
            return false;
        }

        if(!targetEntry_)
        {
            LOG((*logger_), "Null entry was provided!");
            return false;
        }

        const std::string& targetTable = targetEntry_->associatedTable_;
        const std::string& filter = sqlAdapter_->makeFilter(filterInput_);
        std::vector<std::shared_ptr<Types::Entry>> fetchedResult = sqlAdapter_->getEntries(targetTable, filter);
        std::cout << "Found " << fetchedResult.size() << " entries inside the database.\n";
        for(const auto& entryObj : fetchedResult)
        {
            const uint16_t& entryId = entryObj->id_;
            std::cout << entryObj->toString() << "\n";
            if (targetTable == g_tableCourses)
            {
                displayHelper_->displayAttendees(entryId);
            }
            else if (targetTable == g_tableStudents)
            {
                displayHelper_->displayAttendedCourses(entryId);
                displayHelper_->displayGrades(entryId);
            }
        }
        return true;
    }
} // namespace Core::Commands
