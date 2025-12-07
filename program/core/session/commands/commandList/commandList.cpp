#include "commandList.hpp"

namespace Core::Commands
{
    bool CommandList::exec()
    {
        LOG((*logger_), "Executing command LIST");
        std::vector<std::string> tablesNames = sqlAdapter_->getTableNames();
        if (tablesNames.empty())
        {
            LOG((*logger_), "No tables in schema.");
            std::cout << "Could not load schema data information.\n";
            return false;
        }
        for (const auto& name : tablesNames) { std::cout << name << std::endl; }
        return true;
    }
}  // namespace Core::Commands
