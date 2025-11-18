#pragma once

#include "../ICommand.hpp"
#include "../../data/data.hpp"
#include "../../../../types/entry.hpp"
#include "../../../../utilities/sqlite3/sqlAdapter/sqlAdapter.hpp"

#include<memory>

namespace Core::Commands
{
    class CommandExit : public ICommand
    {
    private:
        std::shared_ptr<Utilities::Sql::SqlAdapter> sAdapter_;
        std::shared_ptr<SessionData> sessionData_;
        std::shared_ptr<Entry> newEntry_;

    public:
        inline bool exec() override { return true; }
        inline std::string name() const override { return "exit"; }
        CommandExit(std::shared_ptr<Utilities::Logger> logger) : ICommand(logger) {}
        ~CommandExit() = default;
    };
    
    
} // namespace Core::Commands
