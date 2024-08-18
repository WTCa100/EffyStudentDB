#pragma once
#include <string>
#include <filesystem>
#include <optional>
#include <memory>

#include "../commandHandler/commandHandler.hpp"

namespace Utilities::Workspace
{
    using Utilities::Command::CommandHandler;
    class DirectoryManager
    {
        private:
        std::unique_ptr<CommandHandler> comHandler_;
        const std::filesystem::path appRoot_;
        std::string cwd_;
        
        void setCommandHandler(std::unique_ptr<CommandHandler> newCommandHandler) { comHandler_ = std::move(newCommandHandler); }
        public:
        DirectoryManager(std::string rootPath);
        DirectoryManager();
        ~DirectoryManager() = default;
        inline std::filesystem::path getAppRoot() const { return appRoot_; }
        inline std::string getAppRootStr() const { return appRoot_.string(); }
        bool createDirectory(std::string catalogName, std::optional<std::string> subPath = std::nullopt);
        bool removeDirectory(std::string catalogName, std::optional<std::string> subPath = std::nullopt);
        bool isDirectoryValid(std::string path);
        void getDirectoryContent(std::filesystem::path target);
        // Maybe add std::string/std::vector<std::string> getContent?

    };
    
} // namespace Utilities::Ws
