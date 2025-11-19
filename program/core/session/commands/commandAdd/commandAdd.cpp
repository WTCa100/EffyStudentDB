#include "commandAdd.hpp"

namespace Core::Commands
{
    bool CommandAdd::exec()
    {
        LOG((*logger_), "Executing CommandAdd");
        if (!newEntry_)
        {
            LOG((*logger_), "Provided entry is null!");
            return false;
        }

        if (sAdapter_->addEntry(*newEntry_))
        {
            sessionData_->addEntry(newEntry_);
            return true;
        }
        LOG((*logger_), "Could not add entry: ", newEntry_->toString());
        return false;
    }
}  // namespace Core::Commands
