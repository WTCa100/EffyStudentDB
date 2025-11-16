#pragma once

#include "../../utilities/logger/logger.hpp"
#include "../../utilities/sqlite3/sqlAdapter/sqlAdapter.hpp"
#include "../session/data/data.hpp"

#include <memory>

namespace Core::Display
{
    using Utilities::Logger;
    using Utilities::Sql::SqlAdapter;

    class Helper
    {
      private:
        std::shared_ptr<Logger> logger_;
        std::shared_ptr<SqlAdapter> sAdapter_;
        std::shared_ptr<SessionData> data_;

      public:
        void displayAttendees(const uint16_t& courseId) const;
        void displayAttendedCourses(const uint16_t& studentId) const;
        void displayGrades(const uint16_t& studentId) const;

        explicit Helper(
            std::shared_ptr<Logger> logger, std::shared_ptr<SqlAdapter> sqlAdapter, std::shared_ptr<SessionData> sessionData);
        ~Helper() = default;
    };

}  // namespace Core::Display
