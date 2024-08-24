#pragma once

#include <string>
#include <vector>
#include <map>
#include <filesystem>


#include <sqlite3.h>

namespace Utilities::Workspace
{
    class SqlManager
    {
    private:
        /* data */
        // Map of all studnets
        // Map of all subjects
        // Map of all schools
        // Map of all grades
        void initialValueLoad();
        sqlite3* currentDb_;

    public:
        SqlManager(std::filesystem::path dbPath);
        ~SqlManager();
        bool createSchemaFile();
        bool createTable();
        std::vector<std::string> getEntriesFromTable(std::string tableName, std::vector<std::string> attributes = {});
        bool addEntryToTable(std::string tableName /*, Entry*/);


    };
    
}