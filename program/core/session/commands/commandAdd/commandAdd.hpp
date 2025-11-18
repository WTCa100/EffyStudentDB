#pragma once

#include "../ICommand.hpp"
#include "../../data/data.hpp"
#include "../../../../types/entry.hpp"
#include "../../../../utilities/sqlite3/sqlAdapter/sqlAdapter.hpp"

#include<memory>

namespace Core::Commands
{
    class CommandAdd : public ICommand
    {
    private:
        std::shared_ptr<Utilities::Sql::SqlAdapter> sAdapter_;
        std::shared_ptr<SessionData> sessionData_;
        std::shared_ptr<Entry> newEntry_;

    public:
        bool exec() override;
        inline std::string name() const override { return "add"; } 
        CommandAdd(std::shared_ptr<Utilities::Logger> logger,
                   std::shared_ptr<Utilities::Sql::SqlAdapter> sqlAdapter,
                   std::shared_ptr<SessionData> data, 
                   std::shared_ptr<Entry> newEntry) : ICommand(logger), sAdapter_(sqlAdapter), sessionData_(data), newEntry_(newEntry) {}
        ~CommandAdd() = default;
    };
    
    
} // namespace Core::Commands
