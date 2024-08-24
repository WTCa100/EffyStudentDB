#pragma once

#include <string>
#include <vector>
#include <map>
#include <filesystem>

// Get types
#include "../../../types/school/school.hpp"
#include "../../../types/subject/subject.hpp"
#include "../../../types/student/student.hpp"

#include <sqlite3.h>

namespace Utilities::Workspace
{
    class SqlManager
    {
    private:
        /* data */
        // Map of all subjects
        std::map<uint16_t, subject> subjectList_; 
        // Map of all schools
        std::map<uint16_t, School> schoolList_;
        // Map of all studnets
        std::map<uint16_t, Student> studentList_;
        // Map of all grades
        void initialValuesLoad();
        sqlite3* currentDb_;

    public:
        SqlManager(std::filesystem::path dbPath);
        ~SqlManager();
        bool createSchemaFile();
        bool createTable();
        std::vector<std::string> getEntriesFromTable(std::string tableName, std::vector<std::string> attributes = {});
        bool addEntryToTable(std::string tableName /*, Entry*/);

        static std::vector<std::string> tokenize(std::string rawRow);

        void printSchools();

    };
    
}