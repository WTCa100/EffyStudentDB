#pragma once

#include "ICommand.hpp"
#include "../data/data.hpp"
#include "../../../types/entry.hpp"
#include "../../displayHelper/displayHelper.hpp"
#include "../../../utilities/sqlite3/sqlAdapter/sqlAdapter.hpp"
#include "../../../utilities/inputHandler/inputHandler.hpp"

#include <memory>

namespace Core::Commands
{
    enum class CommandTokensPosition
    {
        type = 0,
        target = 1,
        optionalTag = 2,

        linkTarget = 1,
        linkInvoker = 2,
    };

    class CommandFactory
    {
    private:
        std::shared_ptr<Utilities::Logger> logger_;
        std::shared_ptr<Utilities::Sql::SqlAdapter> sAdapter_;
        std::shared_ptr<SessionData> sessionData_;
        std::shared_ptr<Entry> targetEntry_;
        std::shared_ptr<Display::Helper> displayHelper_;

        bool validateCommand(const std::vector<std::string>& tokenizedCommand);
        static std::shared_ptr<Entry> makeConcreteType(const std::string& tableName); // Isn't that straight up Entry factory?
    public:
        std::unique_ptr<ICommand> makeCommand(const std::string& command);
        CommandFactory(std::shared_ptr<Utilities::Logger> logger,
                       std::shared_ptr<Utilities::Sql::SqlAdapter> sqlAdapter,
                       std::shared_ptr<SessionData> data) : logger_(logger), sAdapter_(sqlAdapter), sessionData_(data), targetEntry_({}), displayHelper_(std::make_shared<Display::Helper>(logger, sqlAdapter, data)) {}
        ~CommandFactory() = default;
    };
    
} // namespace Core::Commands
