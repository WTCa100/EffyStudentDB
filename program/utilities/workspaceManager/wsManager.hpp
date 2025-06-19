#pragma once

#include <filesystem>
#include <map>
#include <memory>
#include <optional>

// Managers
#include "../logger/logger.hpp"
#include "../sqlite3/sqlAdapter/sqlAdapter.hpp"
#include "commandHandler/commandHandler.hpp"
#include "directoryManager/dirManager.hpp"
#include "fileManager/fManager.hpp"

namespace Utilities
{

    using namespace Utilities::Sql;

    class WsManager
    {
      private:
        std::shared_ptr<Utilities::Logger> logger_;
        std::filesystem::path workingDir_{ std::filesystem::current_path() };
        std::unique_ptr<Workspace::DirectoryManager> dManager_;
        std::unique_ptr<Workspace::FileManager> fManager_;
        std::shared_ptr<SqlManager> sManager_;
        void initializeDatabase();
        bool createInitialSchema();
        bool isInitializationNeeded();

      public:
        WsManager(std::string workingDir_);  // For now there is no handling for different working path then this cwd - and
                                             // probably never will be

        WsManager():
            WsManager(std::filesystem::current_path().string())
        {}

        ~WsManager() = default;

        bool createFile(std::string fileName, std::optional<std::filesystem::path> subPath = std::nullopt);
        bool deleteFile(std::string fileName, std::optional<std::filesystem::path> subPath = std::nullopt, bool prompt = true);

        bool fileExists(std::string fileName, std::optional<std::filesystem::path> subPath = std::nullopt);
        bool directoryExists(std::string directoryName, std::optional<std::filesystem::path> subPath = std::nullopt);

        bool createDirectory(std::string directoryName, std::optional<std::filesystem::path> subPath = std::nullopt);
        bool deleteDirectory(
            std::string directoryName, std::optional<std::filesystem::path> subPath = std::nullopt, bool prompt = true);

        std::shared_ptr<Logger> getLogger() const { return logger_; }

        std::shared_ptr<SqlManager> getSqlManager() const { return sManager_; }
    };
}  // namespace Utilities
