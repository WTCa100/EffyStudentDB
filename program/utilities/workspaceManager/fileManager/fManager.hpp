#pragma once

#include <filesystem>
#include <string>
#include <optional>
#include <memory>
#include <fstream>

#include "../commandHandler/commandHandler.hpp"

namespace Utilities::Workspace
{
    using Utilities::Command::CommandHandler;
    class FileManager
    {
        private:
        std::unique_ptr<CommandHandler> comHandler_;
        const std::filesystem::path appRoot_;
        std::filesystem::path cwd_;
        void setCommandHandler(std::unique_ptr<CommandHandler> newCommandHandler) { comHandler_ = std::move(newCommandHandler); }
        public:
        FileManager(std::string rootPath);
        FileManager();
        ~FileManager() = default;

        std::filesystem::path getAppRoot_() const { return appRoot_; }
        std::string getAppRootStr_() const { return appRoot_.string(); }

        bool createFile(std::string fileName, std::optional<std::filesystem::path> subPath = std::nullopt);
        bool deleteFile(std::string fileName, std::optional<std::filesystem::path> subPath = std::nullopt);
        // std::ofstream* openFile(std::string fileName, std::optional<std::filesystem::path> subPath = std::nullopt);
        // ^ either shared or unique
        bool exist(std::string name, std::optional<std::filesystem::path> subPath = std::nullopt);
    };

}