#pragma once

#include "../../../../types/entry.hpp"
#include "../../../../utilities/sqlite3/sqlAdapter/sqlAdapter.hpp"
#include "../../data/data.hpp"
#include "../ICommand.hpp"
#include "../commandFind/commandFind.hpp"

#include <memory>

namespace Core::Commands
{
    class CommandEdit : public ICommand
    {
      private:
        std::shared_ptr<Utilities::Sql::SqlAdapter> sqlAdapter_;
        std::shared_ptr<SessionData> sessionData_;
        const std::shared_ptr<Types::Entry> targetEntry_;
        const std::unordered_map<std::string, std::string> targetFilter_;

      public:
        bool exec() override;

        inline std::string name() const override { return "edit"; }

        CommandEdit(std::shared_ptr<Utilities::Logger> logger,
            std::shared_ptr<Utilities::Sql::SqlAdapter> sqlAdapter,
            std::shared_ptr<SessionData> sessionData,
            const std::shared_ptr<Types::Entry> targetEntry,
            const std::unordered_map<std::string, std::string> targetFilter):
            ICommand(logger),
            sqlAdapter_(sqlAdapter),
            sessionData_(sessionData),
            targetEntry_(targetEntry),
            targetFilter_(targetFilter)
        {}

        ~CommandEdit() = default;
    };
}  // namespace Core::Commands
