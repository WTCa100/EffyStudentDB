#pragma once

#include <filesystem>
#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

// Get types
#include "../../../types/school/school.hpp"
#include "../../../types/student/student.hpp"
#include "../../../types/subject/subject.hpp"
#include "../../../utilities/logger/logger.hpp"
#include "types/attribute/attribute.hpp"
#include "types/table/table.hpp"

#include <sqlite3.h>

namespace Utilities::Sql
{

    typedef std::vector<std::pair<Utilities::Sql::Types::Attribute, std::string>> AttrsValues;
    enum class PragmaTableFormat
    {
        cid        = 0,
        name       = 1,
        type       = 2,
        notnull    = 3,
        dflt_value = 4,
        pk         = 5
    };

    class SqlManager
    {
      private:
        std::unordered_map<std::string, Sql::Types::Table> tables_;
        std::shared_ptr<Logger> logger_;
        std::filesystem::path dbPath_;
        sqlite3* currentDb_;
        bool isDbOpen_;

      public:
        void initialTablesLoad();

        // Low level methods
        std::vector<std::string> executeIn(const std::string& sqlQuery);
        bool executeOut(const std::string& sqlQuery);

        // Wrappers
        bool moveSchemasToDatabase();
        bool moveSchemaToDatabase(const Sql::Types::Table& table);
        bool insertTable(const Sql::Types::Table& newTbl);
        Sql::Types::Table getTableSchema(std::string tableName);

        // Object related methods
        inline void addTable(const Sql::Types::Table& newTbl) { tables_.insert(std::make_pair(newTbl.getName(), newTbl)); }

        const Sql::Types::Table getTable(const std::string& name) const;

        std::unordered_map<std::string, Sql::Types::Table> getTables() const { return tables_; }

        // Move it to adapter
        bool addEntryToTable(std::string tableName, AttrsValues newVals);
        bool updateEntryFromTable(std::string tableName, AttrsValues newVals, std::string condition);
        bool removeEntryFromTable(std::string tableName, uint16_t entryId);
        bool removeEntryFromTable(std::string tableName, std::string condition);
        std::vector<std::string> getEntriesFromTable(std::string tableName,
            std::vector<std::string> attributes = {},
            std::string filter                  = "",
            std::optional<std::string> order    = std::nullopt);

        bool isTableInDatabase(const Sql::Types::Table& table);
        bool isTableInDatabase(const std::string& tableName);

        // bool addEntryToTable(std::string tableName /*, Entry*/);

        SqlManager(std::shared_ptr<Logger> extLogger, std::filesystem::path dbPath);
        ~SqlManager();

        bool openDb();
        void closeDb();

        bool isDbOpen() const { return isDbOpen_; }

        void printTables();
    };

}  // namespace Utilities::Sql