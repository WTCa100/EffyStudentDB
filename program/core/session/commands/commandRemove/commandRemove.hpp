#pragma once

#include "../ICommand.hpp"
#include "../../../../utilities/logger/logger.hpp"
#include "../../../../utilities/sqlite3/sqlAdapter/sqlAdapter.hpp"
#include "../../../../types/entry.hpp"
#include "../../data/data.hpp"

#include <memory>

namespace Core::Commands
{
    class CommandRemove : public ICommand
    {
    private:
        std::shared_ptr<Utilities::Sql::SqlAdapter> sqlAdapter_;
        std::shared_ptr<SessionData> sessionData_;
        std::shared_ptr<Types::Entry> targetEntry_;
    public:
        bool exec() override;
        inline std::string name() const override { return "remove"; }
        CommandRemove(std::shared_ptr<Utilities::Logger> logger,
                      std::shared_ptr<Utilities::Sql::SqlAdapter> sqlAdapter,
                      std::shared_ptr<SessionData> sessionData,
                      std::shared_ptr<Entry> targetEntry) :
                      ICommand(logger),
                      sqlAdapter_(sqlAdapter),
                      sessionData_(sessionData),
                      targetEntry_(targetEntry) {};
        ~CommandRemove() = default;
    };
    
} // namespace Core::Commands
