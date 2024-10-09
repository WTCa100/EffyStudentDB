#pragma once

#include <filesystem>
#include <memory>
#include <optional>
#include <map>

// Managers
#include "directoryManager/dirManager.hpp"
#include "fileManager/fManager.hpp"
#include "../sqlite3/sqlManager/sqlManager.hpp"
#include "commandHandler/commandHandler.hpp"
#include "../logger/logger.hpp"

// Types
#include "../sqlite3/sqlManager/types/attribute/attribute.hpp"
#include "../sqlite3/sqlManager/types/table/table.hpp"
#include "../../types/school/school.hpp"
#include "../../types/student/student.hpp"
#include "../../types/subject/subject.hpp"

namespace Utilities
{

    using namespace Utilities::Workspace::Sql::Types;
    class WsManager
    {
    private:
        std::shared_ptr<Utilities::Logger> logger_;
        std::filesystem::path workingDir_ {std::filesystem::current_path()};
        std::unique_ptr<Workspace::DirectoryManager> dManager_;
        std::unique_ptr<Workspace::FileManager>      fManager_;
        std::unique_ptr<Workspace::SqlManager>       sManager_;
        void initializeDatabase();
        bool createInitialSchema();
        bool isInitializationNeeded();

    public:
        WsManager(std::string workingDir_); // For now there is no handling for different working path then this cwd - and probably never will be
        WsManager() : WsManager(std::filesystem::current_path().string()) {}
        ~WsManager() = default;

        bool createFile(std::string fileName, std::optional<std::filesystem::path> subPath = std::nullopt);
        bool deleteFile(std::string fileName, std::optional<std::filesystem::path> subPath = std::nullopt, bool prompt = true);

        bool fileExists(std::string fileName, std::optional<std::filesystem::path> subPath = std::nullopt);
        bool directoryExists(std::string directoryName, std::optional<std::filesystem::path> subPath = std::nullopt);

        bool createDirectory(std::string directoryName, std::optional<std::filesystem::path> subPath = std::nullopt);
        bool deleteDirectory(std::string directoryName, std::optional<std::filesystem::path> subPath = std::nullopt, bool prompt = true);

        std::shared_ptr<Logger> getLogger() const { return logger_; }

        // Contacting SQL DB
        std::vector<Core::Types::School>      getSchools();
        std::vector<Core::Types::Student>     getStudents();
        std::vector<Core::Types::Subject>     getSubjects();
        std::vector<std::vector<std::string>> getGrades();

        uint16_t getLatestIdFromTable(std::string tblName);

        // Add school
        bool addSchool(Core::Types::School& newSchool);
        bool removeSchool(const Core::Types::School& removeSchool);

        // Add student
        bool addStudent(Core::Types::Student& newStudent);
        bool removeStudent(const Core::Types::Student& targetStudent);
        
        // Add subject
        bool addSubject(Core::Types::Subject& newSubject);
        bool removeSubject(const Core::Types::Subject& targetSubject);
        
        // Add grade
        bool addGrade(Core::Types::Student& targetStudent, Core::Types::Subject& targetSubject, const float& grade);
        bool removeGrade(const Core::Types::Student& targetStudent, const Core::Types::Subject& targetSubject);
        
    };
} // namespace Utilities
