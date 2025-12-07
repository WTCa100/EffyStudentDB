#include "commandRemove.hpp"

#include "../../../../types/entryFactory.hpp"
#include "../../../../utilities/inputHandler/inputHandler.hpp"
namespace Core::Commands
{
    bool CommandRemove::prompt() const
    {
        LOG((*logger_), "Prompting the user to remove entries.");
        std::cout << "Do you wish to remove selected entries?\n";
        return Utilities::InputHandler::getYesOrNo() == 'Y';
    }

    bool CommandRemove::exec()
    {
        const std::string& targetTable = targetEntry_->associatedTable_;
        const std::string& filter      = sqlAdapter_->makeFilter(targetEntry_->userConstruct(false));

        LOG((*logger_), "Executing CommandRemove with the following data: ", targetTable, " filter");
        if (filter.empty())
        {
            LOG((*logger_), "Filter value empty.");
            std::cout << "Fitler value must not be empty!\n";
            return false;
        }

        if (targetTable.empty())
        {
            LOG((*logger_), "Associated table empty.");
            std::cout << "Associated table must not be empty!\n";
            return false;
        }

        std::vector<std::shared_ptr<Types::Entry>> fetchedResult = sqlAdapter_->getEntries(targetTable, filter);
        LOG((*logger_), "Result size = ", fetchedResult.size())
        if (fetchedResult.empty())
        {
            std::cout << "No entries with given filter were found!\n";
            return true;  // No such entries in the database - but this command did not failed.
        }

        std::cout << "Got n = " << fetchedResult.size() << " entries.\n";
        if(prompt())
        {
            for (auto& e : fetchedResult)
            {
                std::cout << "Removing entry with id = " << e->id_ << std::endl;
                if (sqlAdapter_->removeEntry(*e)) { sessionData_->removeEntry(e->id_, targetTable); }
            }
    
            std::cout << "Successfully removed " << fetchedResult.size() << " entries.\n";
        }
        else
        {
            LOG((*logger_), "Command canceled - user aborted.");
            std::cout << "Aborting deletion.\n";
        }
        return true;
    }
}  // namespace Core::Commands
