#pragma once

#include "directoryManager/dirManager.hpp"
#include "fileManager/fManager.hpp"
#include "commandHandler/commandHandler.hpp"
#include "../logger/logger.hpp"

#include <filesystem>
#include <memory>
#include <optional>

namespace Utilities
{

    class WsManager
    {
    private:
        std::shared_ptr<Utilities::Logger> logger_; // No smart pointer - sad
        std::filesystem::path workingDir_ {std::filesystem::current_path()};
        std::unique_ptr<Workspace::DirectoryManager> dManager_;
        std::unique_ptr<Workspace::FileManager>      fManager_;
        bool initializeDatabase();

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

    };
} // namespace Utilities