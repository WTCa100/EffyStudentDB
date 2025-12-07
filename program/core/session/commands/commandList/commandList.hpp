#pragma once

#include "../ICommand.hpp"
#include "../../../../utilities/sqlite3/sqlAdapter/sqlAdapter.hpp"
#include "../../../../utilities/logger/logger.hpp"
#include <memory>

namespace Core::Commands
{
    class CommandList : public ICommand
    {
    private:
        std::shared_ptr<Utilities::Sql::SqlAdapter> sqlAdapter_;
    public:
        bool exec() override;
        inline std::string name() const override { return "list"; }
        CommandList(std::shared_ptr<Utilities::Logger> logger,
                    std::shared_ptr<Utilities::Sql::SqlAdapter> sqlAdapter) :
                    ICommand(logger),
                    sqlAdapter_(sqlAdapter) {};
        ~CommandList() = default;
    };
    
} // namespace Core::Commands
