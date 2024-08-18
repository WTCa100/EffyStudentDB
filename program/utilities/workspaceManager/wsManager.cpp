#include "wsManager.hpp"

namespace Utilities
{
    WsManager::WsManager(std::string workingDir_)
    {
        bool useDefault = false;

        // Check if there are forbidden characters in the provided path
        if(!Workspace::DirectoryManager::isPathGood(workingDir_))
        {
            useDefault = true;
        }

        // Check if given folder exists
        if(!std::filesystem::exists(workingDir_))
        {
            useDefault = true;
        }

        workingDir_ = (useDefault ? std::filesystem::current_path() : std::filesystem::path(workingDir_));
        dManager_ = std::make_unique<Workspace::DirectoryManager>(workingDir_);
        fManager_ = std::make_unique<Workspace::FileManager>(workingDir_);
        std::cout << "WsManager :ctor: specialized - with working directory: " << workingDir_ << "\n";
    }

    bool WsManager::createFile(std::string name, std::optional<std::filesystem::path> subPath)
    {
        // Check if file exists
        // If file exists ask to override
        if(fManager_->exist(name, subPath))
        {
            // For now just abort - don't override
            std::cout << "File: \"" << name << "\" exists at: " << workingDir_.string();
            if(subPath.has_value())
            {
                std::cout << "/" << subPath.value().string();
            } 
            std::cout << "\n";
            std::cout << "Aborting...\n";
            return false;
        }
        // If override - override - return true
        // If not exit - return false
        std::cout << "Creating file: \"" << name << "\" at: " << workingDir_.string();
        if(subPath.has_value())
        {
            std::cout << "/" << subPath.value().string();
        }
        std::cout << "\n";

        return fManager_->createFile(name, subPath);
    }
}