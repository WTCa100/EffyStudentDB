#pragma once

#include "../../../utilities/logger/logger.hpp"
#include "../../../utilities/sqlite3/sqlAdapter/sqlAdapter.hpp"
#include "../data/data.hpp"

#include <memory>

namespace Core
{
    using Utilities::Logger;
    using Utilities::Sql::SqlAdapter;

    class RequestResolver
    {
      private:
        std::shared_ptr<SessionData> sesData_;
        std::shared_ptr<Logger> logger_;
        std::shared_ptr<SqlAdapter> SqlAdapter_;

      public:
        RequestResolver(
            std::shared_ptr<Logger> logger, std::shared_ptr<SqlAdapter> sqlAdapter, std::shared_ptr<SessionData> sesData);
        ~RequestResolver();
    };

}  // namespace Core
