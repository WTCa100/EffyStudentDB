#pragma once

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <filesystem>

// Get types
#include "../../../types/school/school.hpp"
#include "../../../types/subject/subject.hpp"
#include "../../../types/student/student.hpp"
#include "types/attribute/attribute.hpp"
#include "types/table/table.hpp"

#include <sqlite3.h>

namespace Utilities::Workspace
{
    class SqlManager
    {
    private:
        // @Todo move the data lists to a core/dataManagment.hpp/.cpp
        /* data */
        // Map of all subjects
        std::map<uint16_t, subject> subjectList_; 
        // Map of all schools
        std::map<uint16_t, School> schoolList_;
        // Map of all studnets
        std::map<uint16_t, Student> studentList_;
        std::unordered_map<std::string, Sql::Types::Table> tables_;
        
        std::filesystem::path dbPath_;
        sqlite3* currentDb_;
        bool isDbOpen_;
        
        void initialValuesLoad();
    public:
        void initialTablesLoad(std::fstream& schemaPtr);

        // Split calling here
        // Query with vector as a return
        std::vector<std::string> executeIn(const std::string& sqlQuery);
        bool executeOut(const std::string& sqlQuery);

        bool moveSchemasToDatabase();
        bool moveSchemaToDatabase(const Sql::Types::Table& table);

        inline void addTable(const Sql::Types::Table& newTbl) { tables_.insert(std::make_pair(newTbl.getName(), newTbl)); }
        bool insertTable(const Sql::Types::Table& newTbl);
        std::vector<std::string> getEntriesFromTable(std::string tableName, std::vector<std::string> attributes = {}, std::string filter = "");
        Sql::Types::Table getTableSchema(std::string tableName);
        bool isTableInDatabase(const Sql::Types::Table& table);
        bool isTableInDatabase(const std::string& tableName);


        // bool addEntryToTable(std::string tableName /*, Entry*/);

        SqlManager(std::filesystem::path dbPath);
        ~SqlManager();

        bool openDb();
        void closeDb();
        bool isDbOpen() const { return isDbOpen_; }


        void printTables();


    };
    
}