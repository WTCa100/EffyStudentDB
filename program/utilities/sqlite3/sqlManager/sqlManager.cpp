#include "sqlManager.hpp"

#include "../../common/stringManip.hpp"

#include <fstream>
#include <iostream>
#include <sstream>

// @TODO Make a univeresal Sql Call to DB, that would just execute a given command

namespace Utilities::Sql
{
    using namespace Sql::Types;

    std::vector<std::string> SqlManager::executeIn(const std::string& sqlQuery)
    {
        sqlite3_stmt* result;
        LOG((*logger_), "Executing query \"", sqlQuery, "\"");
        if (int rc = !sqlite3_prepare_v2(currentDb_, sqlQuery.c_str(), sqlQuery.size(), &result, nullptr) == SQLITE_OK)
        {
            LOG((*logger_), "Query failed at preparation stage with exit code: ", rc);
            std::cout << "Could not complete your request! Preparation state failed.\n";
            return {};
        }
        LOG((*logger_), "Query prepared without any issues.");

        std::vector<std::string> output;
        int sqlStep;
        while ((sqlStep = sqlite3_step(result)) == SQLITE_ROW)
        {
            std::string rowContent = "";
            for (int i = 0; i < sqlite3_column_count(result); i++)
            {
                const unsigned char* columnInfo = sqlite3_column_text(result, i);

                rowContent += (columnInfo ? reinterpret_cast<const char*>(columnInfo) : "NULL");

                if (i < sqlite3_column_count(result) - 1) { rowContent += "|"; }
            }
            output.push_back(rowContent);
        }

        if (sqlStep != SQLITE_DONE)
        {
            const char* sqlErrMsg = sqlite3_errmsg(currentDb_);
            LOG((*logger_), "Query: \"", sqlQuery, "\" failed! Details: ", sqlErrMsg);
            std::cout << "Could not complete your request! Execution state fail!\n";
        }
        else { LOG((*logger_), "Query \"", sqlQuery, "\" executed without any errors."); }

        sqlite3_finalize(result);
        return output;
    }

    bool SqlManager::executeOut(const std::string& sqlCommand)
    {
        sqlite3_stmt* result;
        LOG((*logger_), "Executing command: \"", sqlCommand, "\"");
        int rc = sqlite3_prepare_v2(currentDb_, sqlCommand.c_str(), sqlCommand.length(), &result, nullptr);
        if (rc != SQLITE_OK)
        {
            LOG((*logger_), "Command failed at preparation stage with exit code: ", rc);
            std::cout << "Could not complete your request! Preparation state failed!\n";
            sqlite3_finalize(result);
            return false;
        }
        LOG((*logger_), "Command prepared without any issues.");

        rc = sqlite3_step(result);
        if (rc != SQLITE_DONE)
        {
            const char* sqlErrMsg = sqlite3_errmsg(currentDb_);
            LOG((*logger_), "Command: \"", sqlCommand, "\" failed with result: ", rc, " Details: ", sqlErrMsg);
            std::cout << "Could not complete your request! Execution state failed.\n";
        }
        else { LOG((*logger_), "Command \"", sqlCommand, "\" executed without any errors."); }

        sqlite3_finalize(result);

        return rc == SQLITE_DONE;
    }

    bool SqlManager::isTableInDatabase(const Sql::Types::Table& table) { return isTableInDatabase(table.getName()); }

    bool SqlManager::isTableInDatabase(const std::string& tableName)
    {
        LOG((*logger_), "Looking for table ", tableName, " in the database");
        std::vector<std::string> presentTables = getEntriesFromTable("sqlite_master", { "name" }, "type = 'table'");
        for (size_t i = 0; i < presentTables.size(); ++i)
        {
            if (presentTables.at(i) == tableName)
            {
                LOG((*logger_), "Found table in the database");
                return true;
            }
        }
        LOG((*logger_), "No such table present in the database")
        return false;
    }

    bool SqlManager::moveSchemasToDatabase()
    {
        LOG((*logger_), "Moving all schemas to database");
        bool isEverythingInserted = false;
        for (const auto& tbl : tables_) { isEverythingInserted = (moveSchemaToDatabase(tbl.second) && isEverythingInserted); }
        return isEverythingInserted;
    }

    bool SqlManager::moveSchemaToDatabase(const Sql::Types::Table& table)
    {
        LOG((*logger_), "Adding new table ", table.getName(), " to database");
        std::cout << "Adding table: " << table.getName() << "\n";
        // Check if a given table exist in the tables map
        if (!tables_.contains(table.getName())) { addTable(table); }

        // @todo check if a given table is already inside .db file

        if (!insertTable(table))
        {
            LOG((*logger_), "Error: Could not insert table!");
            return false;
        }
        LOG((*logger_), "Table has been inserted into the database");
        return true;
    }

    bool SqlManager::insertTable(const Table& newTbl)
    {
        const std::string formula = newTbl.makeFormula();
        return executeOut(formula);
    }

    SqlManager::SqlManager(std::shared_ptr<Logger> extLogger, std::filesystem::path dbPath):
        logger_(extLogger),
        dbPath_(dbPath),
        currentDb_(),
        isDbOpen_(false)
    {}

    bool SqlManager::openDb()
    {
        LOG((*logger_), "Opening db at: ", dbPath_.string());
        if (isDbOpen_)
        {
            std::cout << "Database is already open\n";
            LOG((*logger_), "Database is already open");
            return true;
        }

        int rc = sqlite3_open_v2(dbPath_.c_str(), &currentDb_, SQLITE_OPEN_READWRITE, nullptr);

        if (rc != SQLITE_OK)
        {
            LOG((*logger_), "Sqlite failed to open a db with error code ", rc);
            std::cout << "Sqlite failed to open a db with error code " << rc << "\n";
        }
        else
        {
            std::cout << "Succesully opened DB at: " << dbPath_.string() << "\n";
            LOG((*logger_), "Database at ", dbPath_.string(), " was opened");
            isDbOpen_ = true;
        }
        LOG((*logger_), "OpenDb exits with code: ", rc);
        return rc == SQLITE_OK;
    }

    void SqlManager::closeDb()
    {
        if (isDbOpen_)
        {
            sqlite3_close_v2(currentDb_);
            isDbOpen_ = false;
            LOG((*logger_), "Database at ", dbPath_.string(), " closed");
        }
    }

    const Sql::Types::Table SqlManager::getTable(const std::string& name) const
    {
        if (tables_.contains(name)) { return tables_.at(name); }
        return Table("");
    }

    bool SqlManager::addEntryToTable(std::string tableName, AttrsValues newVals)
    {
        LOG((*logger_), "Adding entry to table ", tableName);
        std::stringstream ss;
        ss << "INSERT INTO " << tableName << "(";
        for (size_t pos = 0; pos < newVals.size(); ++pos)
        {
            const Attribute& attr = newVals.at(pos).first;
            ss << attr.name_;
            if (pos < newVals.size() - 1) { ss << ", "; }
        }
        ss << ") VALUES (";
        for (size_t pos = 0; pos < newVals.size(); ++pos)
        {
            const Attribute& attr = newVals.at(pos).first;
            std::string val       = newVals.at(pos).second;
            if (attr.type_ == AttributeType::SQL_TEXT) { ss << "'" << val << "'"; }
            else { ss << val; }

            if (pos < newVals.size() - 1) { ss << ", "; }
        }
        ss << ");";
        return executeOut(ss.str());
    }

    bool SqlManager::updateEntryFromTable(std::string tableName, AttrsValues newVals, std::string condition)
    {
        LOG((*logger_), "Executing update. Target table: ", tableName, " condition:", condition)
        std::string command = "UPDATE " + tableName + " SET ";
        for (size_t valuePos = 0; valuePos < newVals.size(); valuePos++)
        {
            const Attribute& currentAtr      = newVals.at(valuePos).first;
            const std::string& currentAtrVal = newVals.at(valuePos).second;
            command += currentAtr.name_ + " = \"" + currentAtrVal + "\" ";

            if (valuePos != newVals.size() - 1) { command += ", "; }
        }
        command += "WHERE " + condition + ";";
        return executeOut(command);
    }

    bool SqlManager::removeEntryFromTable(std::string tableName, uint16_t entryId)
    {
        std::string idCondition = "id = " + std::to_string(entryId);
        return removeEntryFromTable(tableName, idCondition);
    }

    bool SqlManager::removeEntryFromTable(std::string tableName, std::string condition)
    {
        std::string command = "DELETE FROM " + tableName + " WHERE " + condition + ";";
        return executeOut(command);
    }

    void SqlManager::initialTablesLoad()
    {
        LOG((*logger_), "Initializing all tables from the schema file");
        // Get all table names from the DB
        std::vector<std::string> tableNames = getEntriesFromTable("sqlite_master", { "name" }, "type = 'table'");

        LOG((*logger_), "Got ", tableNames.size(), " tables");
        for (auto name : tableNames) { addTable(getTableSchema(name)); }
        LOG((*logger_), "Tables have been added into the tables_");
    }

    /// @brief This function will perform a SELECT statement with given parameters.
    /// @param tableName from which table entries will be fetched.
    /// @param attributes what attributes are desired to be fetched.
    /// @param filter what filter should narrow the SELECT result. No filter means every entry will get fetched.
    /// @param order how they should be ordered. This field shall only contain information about "<field_name> <order_type>"
    /// @note By default every select is ordered by table primary key in ascending manner
    /// @return List of every fetched entry in string format
    std::vector<std::string> SqlManager::getEntriesFromTable(
        std::string tableName, std::vector<std::string> attributes, std::string filter, std::optional<std::string> order)
    {
        LOG((*logger_), "Fetching entries from table: ", tableName, " attributes count: ", attributes.size(),
            " filter: ", filter);
        std::string selectWhat = "";
        std::string sqlFormat  = "SELECT ";
        if (attributes.empty()) { selectWhat = "*"; }
        else
        {
            for (size_t i = 0; i < attributes.size(); i++)
            {
                selectWhat += attributes.at(i);
                if (i < attributes.size() - 1) { selectWhat += ","; }
            }
        }

        sqlFormat += selectWhat + " FROM " + tableName;

        if (!filter.empty()) { sqlFormat += " WHERE " + filter; }
        if (order.has_value()) { sqlFormat += " ORDER BY " + order.value(); }
        sqlFormat += ";";

        return executeIn(sqlFormat);
    }

    Sql::Types::Table SqlManager::getTableSchema(std::string tableName)
    {
        LOG((*logger_), "Getting table \"", tableName, "\" schema");
        // First check if we got the table already in memory.
        if (tables_.contains(tableName))
        {
            LOG((*logger_), "Table already in memory");
            return tables_.at(tableName);
        }

        // The output of this pragma is cid|name|type|notnull|dflt_value|pk
        std::string commandAttr = "PRAGMA table_info(" + tableName + ");";
        // The output of this pragma is id|seq|table|from|to|on_update|on_delete|match
        std::string commandForKeys = "PRAGMA foreign_key_list(" + tableName + ");";

        Table resultTbl(tableName);

        std::vector<std::string> outputAttr  = executeIn(commandAttr);
        std::vector<std::string> outputFkeys = executeIn(commandForKeys);

        LOG((*logger_), "Mapping attributes");
        for (const auto& attr : outputAttr)
        {
            std::vector<std::string> tokenizedAttr = Common::tokenize(attr, '|');
            Attribute finalAttr;
            for (size_t elementId = 1; elementId < tokenizedAttr.size(); elementId++)
            {
                switch (static_cast<PragmaTableFormat>(elementId))
                {
                    case PragmaTableFormat::name : finalAttr.name_ = tokenizedAttr.at(elementId); break;
                    case PragmaTableFormat::type :
                        finalAttr.type_ = Utilities::Sql::stringToAttrType(tokenizedAttr.at(elementId));
                        break;
                    case PragmaTableFormat::notnull :
                        if (tokenizedAttr.at(elementId) == "1") { finalAttr.flags_.insert(AttributeFlag::NOT_NULL); }
                        break;
                    case PragmaTableFormat::dflt_value :
                        if (tokenizedAttr.at(elementId) != "NULL")
                        {
                            finalAttr.flags_.insert(AttributeFlag::DEFAULT);
                            finalAttr.defaultValue_ = tokenizedAttr.at(elementId);
                        }
                        break;
                    case PragmaTableFormat::pk :
                        if (tokenizedAttr.at(elementId) == "1") { finalAttr.flags_.insert(AttributeFlag::PRIMARY_KEY); }
                        break;
                    default : std::cout << "Unknown element id - skipping\n"; break;
                }
            }
            resultTbl.addToSchema(std::move(finalAttr));
        }

        if (!outputFkeys.empty())
        {
            LOG((*logger_), "Mapping foreign kets");
            for (const auto& fKey : outputFkeys)
            {
                std::vector<std::string> tokenizedKeys = Common::tokenize(fKey, '|');
                Attribute linkAttr                     = resultTbl.getAttributeByName(tokenizedKeys.at(3));
                if (!linkAttr.isValid())
                {
                    std::cout << "No such attribute " << tokenizedKeys.at(3) << " in Table " << resultTbl.getName()
                              << ". Skipping entry...\n";
                    continue;
                }
                resultTbl.linkAttributes(linkAttr, tokenizedKeys.at(5), tokenizedKeys.at(4));
            }
        }
        LOG((*logger_), "Returning table: name=", resultTbl.getName(), " attrCount=", resultTbl.getSchema().size(),
            " foreignKeyCount=", resultTbl.getForeignKeys().size());
        return resultTbl;
    }

    SqlManager::~SqlManager()
    {
        LOG((*logger_), "SqlManager :dtor:");
        closeDb();
    }

    void SqlManager::printTables()
    {
        LOG((*logger_), "Trying to print all tables");
        if (tables_.empty())
        {
            std::cout << "No tables present\n";
            return;
        }

        for (const auto& tbl : tables_) { std::cout << tbl.second.makeFormula() << "\n"; }
    }

}  // namespace Utilities::Sql
