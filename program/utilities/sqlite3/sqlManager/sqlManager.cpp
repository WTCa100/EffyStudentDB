#include <iostream>
#include <sstream>

#include "sqlManager.hpp"

namespace Utilities::Workspace
{
    SqlManager::SqlManager(std::filesystem::path dbPath)
    {
        std::cout << "SQL Manager :ctor:\n";
        sqlite3_open_v2(dbPath.c_str(), &currentDb_, SQLITE_OPEN_READWRITE, nullptr);
    }

    std::vector<std::string> SqlManager::getEntriesFromTable(std::string tableName, std::vector<std::string> attributes)
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

        sqlite3_stmt *result;
        sqlFormat += selectWhat + " FROM " + tableName + ";";
        std::cout << "Executing command: \"" << sqlFormat << "\n"; 
        if(int rc = !sqlite3_prepare_v2(currentDb_, sqlFormat.c_str(), sqlFormat.size(), &result, nullptr) == SQLITE_OK)
        {
            std::cout << "Command failed with exit code: " << rc << "\n";
            return {};
        }
        std::cout << "Command prepared without any errors!\n";

        std::vector<std::string> output;
        while(sqlite3_step(result) == SQLITE_ROW)
        {
            std::string rowContent = "";
            for(int i = 0; i < sqlite3_column_count(result); i++)
            {
                
                const unsigned char *columnInfo = sqlite3_column_text(result, i);

                rowContent += (columnInfo ? reinterpret_cast<const char*>(columnInfo) : "NULL");

                if(i < sqlite3_column_count(result) - 1)
                {
                    rowContent += ", ";
                }
            }
            output.push_back(rowContent);
        }
        return output;
    }

    SqlManager::~SqlManager()
    {
        std::cout << "SqlManager :dtor:\n";
        if(currentDb_)
        {
            sqlite3_close_v2(currentDb_);
        }
    }

} // namespace Utilities::Sql
