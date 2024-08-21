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

        // For now - will make this prettier later
        return fManager_->createFile(name, subPath);
    }

    bool WsManager::fileExists(std::string fileName, std::optional<std::filesystem::path> subPath)
    {
        std::cout << "Looking for the file " << fileName << "\n";
        bool rc = fManager_->exist(fileName, subPath);

        std::cout << rc ? ("File: " + fileName + " was found!\n") 
                        : ("Could not find file: " + fileName + "\n");
        return rc;
    }


    bool WsManager::deleteFile(std::string fileName, std::optional<std::filesystem::path> subPath, bool prompt)
    {

        // First check if exists
        if(prompt)
        {
            std::cout << "This function will permamently delete given file. This cannot be undone!\n";
            std::cout << "Are you sure you want to continue and delete " << fileName << "?\n";
            std::string tmp;
            std::getline(std::cin, tmp);
            if(tmp != "yes")
            {
                std::cout << "Aborting...\n";
                return false;
            }
        }

        return fManager_->deleteFile(fileName, subPath);

    }


    bool WsManager::createDirectory(std::string directoryName, std::optional<std::filesystem::path> subPath)
    {
        std::cout << "Creating directory: " << directoryName << "\n";
        // Check for folder
        if(dManager_->exist(directoryName, subPath))
        {
            std::cout << "Folder already exists! Aborting...\n";
            return false;
        }

        // For now - will make this prettier later
        return dManager_->createDirectory(directoryName, subPath);
    }

    bool WsManager::directoryExists(std::string directoryName, std::optional<std::filesystem::path> subPath)
    {
        std::cout << "Looking for the directory: " << directoryName << "...\n";
        bool rc = dManager_->exist(directoryName, subPath);

        std::cout << rc ? ("File: " + directoryName + " was found!\n") 
                        : ("Could not find file: " + directoryName + "\n");

        return rc;
    }

    bool WsManager::deleteDirectory(std::string directoryName, std::optional<std::filesystem::path> subPath, bool prompt)
    {

        // Check if a file exists - if there is no file, there is no need on doing anythin
        if(!dManager_->exist(directoryName, subPath))
        {
            std::cout << "No such file in directory! Skipping...\n";
            return true;
        }

        if(prompt)
        {
            std::cout << "This function will permamently delete given folder and it's content. This cannot be undone!\n";
            std::cout << "Are you sure you want to continue and delete " << directoryName << "?\n";
            std::string tmp;
            std::getline(std::cin, tmp);
            if(tmp != "yes")
            {
                std::cout << "Aborting...\n";
                return false;
            }
        }

        std::cout << "Attempting to remove " << directoryName << "...\n";
        uint16_t tryCount = 0;
        bool hasDeleted = false;
        do
        {
            ++tryCount;
            std::cout << "Try " << tryCount << " result:";
            hasDeleted = dManager_->deleteDirectory(directoryName, subPath);
            std::cout << (hasDeleted ? " File was deleted\n" : "Could not delete file\n");
        } while (!hasDeleted && tryCount <= 3);
        

        std::cout << hasDeleted ? ("Directory " + directoryName + " was deleted\n") : ("Could not delete directory " + directoryName + ".\n");
        return hasDeleted;
    }

}