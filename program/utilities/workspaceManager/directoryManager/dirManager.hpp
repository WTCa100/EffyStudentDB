#pragma once
#include <string>
#include <filesystem>
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
        const std::string           appRootStr_;
        
        void setCommandHandler(std::unique_ptr<CommandHandler> newCommandHandler) { comHandler_ = std::move(newCommandHandler); }
        public:
        DirectoryManager(std::string rootPath);
        DirectoryManager();
        ~DirectoryManager();
        inline std::filesystem::path getAppRoot() const { return appRoot_; }
        inline std::string getAppRootStr() const { return appRoot_.string(); }
        bool createDirectory(std::string catalogName);
        bool removeDirectory(std::string catalogName);
        bool isDirectoryValid(std::string path);
        void showContent(std::filesystem::path target);

    };
    
} // namespace Utilities::Ws
