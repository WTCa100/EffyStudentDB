#pragma once

#include <filesystem>
#include <memory>
#include <optional>

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

        // Get Schools
        std::vector<Core::Types::School> getSchools();
        // Get Students
        // Get subjects
        // Get grades

        // Add school
        // Add student
        // Add subject
        // Add grade
    };
} // namespace Utilities