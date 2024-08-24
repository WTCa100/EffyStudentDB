#include <iostream>
#include <sstream>

#include "sqlManager.hpp"

namespace Utilities::Workspace
{
    SqlManager::SqlManager(std::filesystem::path dbPath)
    {
        std::cout << "SQL Manager :ctor:\n";
        sqlite3_open_v2(dbPath.c_str(), &currentDb_, SQLITE_OPEN_READWRITE, nullptr);
        initialValuesLoad();
    }

    void SqlManager::initialValuesLoad()
    {
        // Handle Subjects
        {
            std::vector<std::string> initialSchools = getEntriesFromTable("Subjects");
            for(const auto& subjectRaw : initialSchools)
            {
                std::vector<std::string> processedEntry = tokenize(subjectRaw);
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
                std::vector<std::string> processedEntry = tokenize(schoolRow);
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
                std::vector<std::string> processedEntry = tokenize(studentRow);
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
                    rowContent += "|";
                }
            }
            output.push_back(rowContent);
        }

        sqlite3_finalize(result);
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

    std::vector<std::string> SqlManager::tokenize(std::string rawRow)
    {
        std::vector<std::string> tokens;
        const char delim = '|';
        std::string token;
        std::istringstream iss(rawRow);

        while(std::getline(iss, token, delim))
        {
            tokens.push_back(token);
        }
        return tokens;
    }

    void SqlManager::printSchools()
    {
        std::cout << "Showing schools:\n";
        for(auto s : schoolList_)
        {
            std::cout << s.first << ": " << s.second.name_ << "\n";
            std::cout << "Student List:\n";
            if(s.second.students_.empty())
            {
                std::cout << "None!\n";
            }
            else
            {
                for(auto student : s.second.students_)
                {
                    std::cout << student.firstName_ << " " << student.lastName_ << "\n";
                }
            }
        }
    }

} // namespace Utilities::Sql
