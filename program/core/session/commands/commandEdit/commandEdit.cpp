#include "commandEdit.hpp"

#include "../../../../types/entryFactory.hpp"

namespace Core::Commands
{
    bool CommandEdit::exec()
    {
        const std::string& targetTable = targetEntry_->associatedTable_;
        const std::string& filter = sqlAdapter_->makeFilter(targetFilter_);

        LOG((*logger_), "Executing CommandEdit with the following data: ", targetTable, " filter");
        if(targetFilter_.empty())
        {
            LOG((*logger_), "Filter value empty.");
            std::cout << "Fitler value must not be empty!\n";
            return false;
        }

        if(targetFilter_.empty())
        {
            LOG((*logger_), "Associated table empty.");
            std::cout << "Associated table must not be empty!\n";
            return false;
        }

        std::vector<std::shared_ptr<Types::Entry>> fetchedResult = sqlAdapter_->getEntries(targetTable, filter);
        LOG((*logger_), "Result size = ", fetchedResult.size())
        if(fetchedResult.empty())
        {
            std::cout << "No entries with given filter were found!\n";
            return true; // No such entries in the database - but this command did not failed.
        }

        std::cout << "Got n = " << fetchedResult.size() << " entries.\n";
        std::shared_ptr<Entry> updatedValues = Core::Types::EntryFactory::getEntry(targetTable);
        std::unordered_map<std::string, std::string> changesToCommit = updatedValues->userConstruct(false);
        for(auto& e : fetchedResult)
        {
            std::cout << "Updating entry with id = " << e->id_ << std::endl;
            std::shared_ptr<Entry> currentEntry = Core::Types::EntryFactory::getEntry(targetTable);
            currentEntry = updatedValues->mirrorMissing(e);
            if (sqlAdapter_->updateEntry(e->id_, changesToCommit, targetTable))
            {
                sessionData_->updateEntry(e->id_, currentEntry);
            }
        }

        return true;
    }
} // namespace Core::Commands
