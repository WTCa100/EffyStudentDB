#pragma once

#include <string>
#include <vector>
#include <map>
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
        std::map<std::string, Sql::Types::Table> tables_;
        sqlite3* currentDb_;
        std::filesystem::path dbPath_;
        void initialValuesLoad();
    public:
        bool addInitialSchema(std::fstream* fPtr);
        bool initializeDatabase();

        inline void addTable(const Sql::Types::Table& newTbl) { tables_.insert(std::make_pair(newTbl.getName(), newTbl)); }
        bool insertTable(const Sql::Types::Table& newTbl);
        std::vector<std::string> getEntriesFromTable(std::string tableName, std::vector<std::string> attributes = {});
        // Sql::Types::Table getTableSchema(std::string tableName);
        // bool addEntryToTable(std::string tableName /*, Entry*/);
        
        SqlManager(std::filesystem::path dbPath);
        ~SqlManager();

        static std::vector<std::string> tokenize(std::string rawFormat);

        bool openDb();
        void closeDb();


    };
    
}