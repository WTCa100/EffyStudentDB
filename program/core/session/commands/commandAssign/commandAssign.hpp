#pragma once

#include "../../../../utilities/sqlite3/sqlAdapter/sqlAdapter.hpp"
#include "../ICommand.hpp"

#include <memory>

namespace Core::Commands
{
    class CommandAssign : public ICommand
    {
      private:
        std::shared_ptr<Utilities::Sql::SqlAdapter> sAdapter_;
        const uint16_t targetCourseId_;
        const uint16_t targetStudentId_;

      public:
        bool exec() override;

        inline std::string name() const override { return "assign"; }

        CommandAssign(std::shared_ptr<Utilities::Logger> logger,
            std::shared_ptr<Utilities::Sql::SqlAdapter> sqlAdapter,
            const uint16_t targetCourseId,
            const uint16_t targetStudentId):
            ICommand(logger),
            sAdapter_(sqlAdapter),
            targetCourseId_(targetCourseId),
            targetStudentId_(targetStudentId) {};
        ~CommandAssign() = default;
    };

}  // namespace Core::Commands
