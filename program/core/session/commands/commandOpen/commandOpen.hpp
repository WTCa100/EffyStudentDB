#pragma once

#include "../../../../utilities/sqlite3/sqlAdapter/sqlAdapter.hpp"
#include "../../data/data.hpp"
#include "../ICommand.hpp"

#include <memory>

namespace Core::Commands
{ 
    class CommandOpen : public ICommand
    {
      private:
        std::shared_ptr<SessionData> sessionData_;
        std::shared_ptr<Utilities::Sql::SqlAdapter> sAdapter_;
        const uint16_t targetCourseId_;
      public:
        bool exec() override;

        inline std::string name() const override { return "close"; }

        CommandOpen(std::shared_ptr<Utilities::Logger> logger,
            std::shared_ptr<SessionData> sessionData,
            std::shared_ptr<Utilities::Sql::SqlAdapter> sqlAdapter,
            const uint16_t targetCourseId):
            ICommand(logger),
            sessionData_(sessionData),
            sAdapter_(sqlAdapter),
            targetCourseId_(targetCourseId) {};
        ~CommandOpen() = default;
    };
    
} // namespace Core::Commands
