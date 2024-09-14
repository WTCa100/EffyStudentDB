#include <iostream>
#include <sstream>
#include <fstream>

#include "sqlManager.hpp"
#include "../../common/stringManip.hpp"

// @TODO Make a univeresal Sql Call to DB, that would just execute a given command

namespace Utilities::Workspace
{
    using namespace Sql::Types;

    std::vector<std::string> SqlManager::executeIn(const std::string& sqlQuery)
    {
        sqlite3_stmt *result;
        std::cout << "Executing query: \"" << sqlQuery << "\n"; 
        if(int rc = !sqlite3_prepare_v2(currentDb_, sqlQuery.c_str(), sqlQuery.size(), &result, nullptr) == SQLITE_OK)
        {
            std::cout << "Command failed with exit code: " << rc << "\n";
            return {};
        }
        std::cout << "Command prepared without any errors!\n";

        std::vector<std::string> output;
        int sqlStep;
        while((sqlStep = sqlite3_step(result)) == SQLITE_ROW)
        {
            std::string rowContent = "";
            for(int i = 0; i < sqlite3_column_count(result); i++)
            {
                
                const unsigned char *columnInfo = sqlite3_column_text(result, i);

                rowContent += (columnInfo ? reinterpret_cast<const char*>(columnInfo) : "NULL");

                if(i < sqlite3_column_count(result) - 1)
                {
                    rowContent += "|";
                }
            }
            output.push_back(rowContent);
        }

        if(sqlStep != SQLITE_DONE)
        {
            std::cout << "Command failed with code " << sqlStep << "\n";
            std::cout << "Details: " << sqlite3_errmsg(currentDb_) << "\n";
        }
        else
        {
            std::cout << "Command executed without any errores!\n";
        }

        sqlite3_finalize(result);
        return output;
    }

    bool SqlManager::executeOut(const std::string& sqlCommand)
    {
        sqlite3_stmt *result;
        std::cout << "Executing command: \"" << sqlCommand << "\"\n"; 
        int rc = sqlite3_prepare_v2(currentDb_, sqlCommand.c_str(), sqlCommand.length(), &result, nullptr);
        if(rc != SQLITE_OK)
        {
            std::cout << "Command failed with exit code: " << rc << "\n";
            sqlite3_finalize(result);
            return false;
        }
        std::cout << "Command prepared without any errors!\n";

        rc = sqlite3_step(result);
        if(rc != SQLITE_DONE)
        {
            std::cout << "Command failed with code " << rc << "\n";
            std::cout << "Details: " << sqlite3_errmsg(currentDb_) << "\n";
        }
        else
        {
            std::cout << "Command executed without any errors!\n";
        }

        sqlite3_finalize(result);

        return rc == SQLITE_DONE;
    }

    bool SqlManager::isTableInDatabase(const Sql::Types::Table& table)
    {
        return isTableInDatabase(table.getName());
    }

    bool SqlManager::isTableInDatabase(const std::string& tableName)
    {
        std::vector<std::string> presentTables = getEntriesFromTable("sqlite_master", {"name"}, "type = 'table'");
        for(size_t i = 0; i < presentTables.size(); ++i)
        {
            if(presentTables.at(i) == tableName)
            {
                return true;
            }
        }
        return false;
    }

    bool SqlManager::moveSchemasToDatabase()
    {
        bool isEverythingInserted = false;
        for(const auto& tbl : tables_)
        {
            // THIS IS SO BAD FIX IT!!!
            isEverythingInserted = (moveSchemaToDatabase(tbl.second) && isEverythingInserted);
        }
        return isEverythingInserted;
    }

    bool SqlManager::moveSchemaToDatabase(const Sql::Types::Table& table)
    {
        std::cout << "Adding table: " << table.getName() << "\n";
        // Check if a given table exist in the tables map
        if(!tables_.contains(table.getName()))
        {
            addTable(table);
        }

        // @todo check if a given table is already inside .db file

        if(!insertTable(table))
        {
            return false;
        }
        return true;
    }

    bool SqlManager::insertTable(const Table& newTbl)
    {
        const std::string formula = newTbl.makeFormula();
        return executeOut(formula);
    }

    SqlManager::SqlManager(std::filesystem::path dbPath) : subjectList_({}), schoolList_({}), studentList_({}),dbPath_(dbPath), currentDb_(), isDbOpen_(false) {}

    bool SqlManager::openDb()
    {
        if(isDbOpen_)
        {
            std::cout << "Database is already open\n";
            return true;
        }

        int rc = sqlite3_open_v2(dbPath_.c_str(), &currentDb_, SQLITE_OPEN_READWRITE, nullptr);
        
        if(rc != SQLITE_OK)
        {
            std::cout << "Sqlite failed to opend a db with error code " << rc << "\n";
        }
        else
        {
            std::cout << "Succesully opened DB at: " << dbPath_.string() << "\n";
            isDbOpen_ = true;
        }
        return rc == SQLITE_OK;
    }

    void SqlManager::closeDb()
    {
        if(isDbOpen_)
        {
            sqlite3_close_v2(currentDb_);
            isDbOpen_ = false;
        }
    }

    // This has to be deleted from the sql manager and moved into a either data manager/data organizer OR
    // into a SQL Adapter. Generaly all of the data will be inside the core functionality and will as WsManager
    // to get entires from a given table.
    void SqlManager::initialValuesLoad()
    {
        // Handle Subjects
        {
            std::vector<std::string> initialSchools = getEntriesFromTable("Subjects");
            for(const auto& subjectRaw : initialSchools)
            {
                std::vector<std::string> processedEntry = Utilities::Common::tokenize(subjectRaw, '|');
                // Id
                auto subjectId = static_cast<uint16_t>(std::stoi(processedEntry.at(0)));
                // Name
                std::string subjectName = processedEntry.at(1);
                subjectList_.insert(std::make_pair(subjectId, subjectName));
            }
        }
        // Handle Schools
        {
            std::vector<std::string> initialSchools = getEntriesFromTable("Schools");
            for(const auto& schoolRow : initialSchools)
            {
                std::vector<std::string> processedEntry = Utilities::Common::tokenize(schoolRow, '|');
                // Id
                auto schoolId = static_cast<uint16_t>(std::stoi(processedEntry.at(0)));
                // Name
                std::string schoolName = processedEntry.at(1);

                schoolList_.insert(std::make_pair(schoolId, School{schoolId, schoolName, {}}));
            }
        }
        // Handle Students
        {
            std::vector<std::string> initialStudents = getEntriesFromTable("Students");
            for(const auto& studentRow : initialStudents)
            {
                std::vector<std::string> processedEntry = Utilities::Common::tokenize(studentRow, '|');
                // Id
                auto studentId = static_cast<uint16_t>(std::stoi(processedEntry.at(0)));

                // First name
                std::string firstName = processedEntry.at(1);

                // Second name
                std::optional<std::string> secondName;
                std::string rawSndName = processedEntry.at(2);
                if(rawSndName == "NONE")
                {
                    secondName = std::nullopt;
                }
                else
                {
                    secondName = rawSndName;
                }

                // Last name
                std::string lastName = processedEntry.at(3);
                Student finalEntry{studentId, firstName, secondName, lastName, {}};

                studentList_.insert(std::make_pair(studentId, finalEntry));

                // School - assigning given student to a school from the ID
                auto schoolId = static_cast<uint16_t>(std::stoi(processedEntry.at(4)));
                if(schoolList_.contains(schoolId))
                {
                    schoolList_.at(schoolId).students_.push_back(finalEntry);
                }
            }
        }
        // Handle Grades

        // std::cout << "Showing subjects:\n";
        // for(auto s : subjectList_)
        // {
        //     std::cout << "Subject : " << s.first << ":" << s.second << "\n";
        // }

        // std::cout << "Showing Students:\n";
        // for(auto s : studentList_)
        // {
        //     std::cout << "Student: " << s.first << ": " << s.second.firstName_ << " " << s.second.lastName_ << "\n";
        // }
    }

    bool SqlManager::addEntryToTable(std::string tableName, entry newVals)
    {
        std::stringstream ss;
        ss << "INSERT INTO " << tableName << "(";
        for(size_t pos = 0; pos < newVals.size(); ++pos)
        {
            const Attribute& attr = newVals.at(pos).first;
            ss << attr.name_;
            if(pos < newVals.size() - 1)
            {
                ss << ", ";
            }
        }
        ss << ") VALUES (";
        for(size_t pos = 0; pos < newVals.size(); ++pos)
        {
            const Attribute& attr = newVals.at(pos).first;
            std::string val = newVals.at(pos).second;
            if(attr.type_ == "TEXT")
            {
                ss << "'" << val << "'";
            }
            else
            {
                ss << val;
            }

            if(pos < newVals.size() - 1)
            {
                ss << ", ";
            }
        }
        ss << ");";
        return executeOut(ss.str());
    }

    bool SqlManager::removeEntryFromTable(std::string tableName, uint16_t entryId)
    {
        std::string command =  "DELETE FROM " + tableName + " WHERE id = " + std::to_string(entryId) +";";
        return executeOut(command);
    }

    // @TODO removeEntryFromTable with custom condition string
    // removeEntryFromTable(string [tableName], string [condition])

    void SqlManager::initialTablesLoad(std::fstream& schemaPtr)
    {
        std::cout << "Loading all tables into the memory...\n";
        // Get all table names from the DB
        std::vector<std::string> tableNames = getEntriesFromTable("sqlite_master", {"name"}, "type = 'table'");

        std::cout << "Got the following names:\n";
        for(auto name : tableNames)
        {
            addTable(getTableSchema(name));
        }
        std::cout << "Initial tables has been loaded\n";
    }

    std::vector<std::string> SqlManager::getEntriesFromTable(std::string tableName, std::vector<std::string> attributes, std::string filter)
    {
        std::string selectWhat = "";
        std::string sqlFormat = "SELECT ";
        if(attributes.empty())
        {
            selectWhat = "*";
        }
        else
        {
            for(size_t i = 0; i < attributes.size(); i++)
            {
                selectWhat += attributes.at(i);
                if(i < attributes.size() - 1)
                {
                    selectWhat += ",";
                }
            }
        }

        sqlFormat += selectWhat + " FROM " + tableName;

        if(!filter.empty())
        {
            sqlFormat += " WHERE " + filter;
        }
        sqlFormat += ";";

        return executeIn(sqlFormat);
    }

    Sql::Types::Table SqlManager::getTableSchema(std::string tableName)
    {
        // The output of this pragma is cid|name|type|notnull|dflt_value|pk
        std::string commandAttr = "PRAGMA table_info(" + tableName + ");";
        // The output of this pragma is id|seq|table|from|to|on_update|on_delete|match
        std::string commandForKey = "PRAGMA foreign_key_list(" + tableName + ");";
        
        Table resultTbl(tableName);
        
        std::vector<std::string> outputAttr = executeIn(commandAttr);
        std::vector<std::string> outputFkeys = executeIn(commandForKey);


        for(const auto& attr : outputAttr)
        {
            std::vector<std::string> tokenizedAttr = Common::tokenize(attr, '|');
            Attribute finalAttr;
            for(size_t elementId = 1; elementId < tokenizedAttr.size(); elementId++)
            {
                switch (static_cast<PragmaTableFormat>(elementId))
                {
                case PragmaTableFormat::name:
                    finalAttr.name_ = tokenizedAttr.at(elementId);
                    break;
                case PragmaTableFormat::type:
                    finalAttr.type_ = tokenizedAttr.at(elementId);
                case PragmaTableFormat::notnull:
                    if(tokenizedAttr.at(elementId) == "1")
                    {
                        finalAttr.flags_.push_back(AttributeFlag::NOT_NULL);
                    }
                    break;
                case PragmaTableFormat::dflt_value:
                    if(tokenizedAttr.at(elementId) != "NULL")
                    {
                        std::cout << "Default value is not supported yet - skipping.\n";
                    }
                    break;
                case PragmaTableFormat::pk:
                    if(tokenizedAttr.at(elementId) == "1")
                    {
                        finalAttr.flags_.push_back(AttributeFlag::PRIMARY_KEY);
                    }
                    break;
                default:
                    std::cout << "Unknown element id - skipping\n";
                    break;
                }
            }
            resultTbl.addToSchema(std::move(finalAttr));
        }
        return resultTbl;
    }

    SqlManager::~SqlManager()
    {
        std::cout << "SqlManager :dtor:\n";
        closeDb();
    }

    void SqlManager::printTables()
    {
        if(tables_.empty())
        {
            std::cout << "No tables present\n";
            return;
        }

        for(const auto& tbl : tables_)
        {
            std::cout << tbl.second.makeFormula() << "\n";
        }
    }

} // namespace Utilities::Sql
