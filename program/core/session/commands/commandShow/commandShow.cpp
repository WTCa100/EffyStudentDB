#include "commandShow.hpp"

#include <sstream>

namespace Core::Commands
{
    void CommandShow::showEntry(const uint16_t& id) const
    {
        std::shared_ptr<Entry> entry = sessionData_->getEntry(id, target_);
        if(!entry)
        {
            std::stringstream errorMsg;
            errorMsg << "Entry with id " << id << " does not exists but was provided by sessionData!"; 
            throw std::runtime_error(errorMsg.str());
        }
        std::cout << entry->toString();

        if(target_ == g_tableCourses) { displayHelper_->displayAttendees(entry->id_); }
        else if(target_ == g_tableStudents)
        {
            displayHelper_->displayAttendedCourses(id);
            displayHelper_->displayGrades(id);
        }
        std::cout << std::endl;
    }

    bool CommandShow::exec()
    {
        std::shared_ptr<abstractTypeList> targetTableData = sessionData_->getEntries(target_);

        if(!targetTableData)
        {
            LOG((*logger_), "Got invalid table from target = ", target_);
            return false;
        }

        for(const auto& dataEntry : *targetTableData)
        {
            const uint16_t& id = dataEntry.first;
            showEntry(id);
        }
        return false;
    }
} // namespace Core::Commands
