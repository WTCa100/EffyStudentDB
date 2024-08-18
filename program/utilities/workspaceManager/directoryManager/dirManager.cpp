#include <iostream>
#include <cstring>

#include "dirManager.hpp"

#include "../commandHandler/createCommand/createCommand.hpp"
#include "../commandHandler/verifyCommand/verifyCommand.hpp"
#include "../commandHandler/removeCommand/removeCommand.hpp"

using namespace Utilities::Command;
namespace Utilities::Workspace
{

    DirectoryManager::DirectoryManager(std::string rootPath) : appRoot_(std::filesystem::path(rootPath)), cwd_(appRoot_)
    {
        std::cout << "DirManager :ctor: appRoot_=" << appRoot_.string() << " (from specialized ctor)\n";
    }

    DirectoryManager::DirectoryManager() : DirectoryManager(std::filesystem::current_path()) {}

    bool DirectoryManager::createDirectory(std::string catalogName, std::optional<std::string> subPath)
    {
        setCommandHandler(std::make_unique<CreateCommand>(catalogName, targetType::directory, subPath));
        return comHandler_->execute();
    }

    bool DirectoryManager::removeDirectory(std::string catalogName, std::optional<std::string> subPath)
    {
        unsigned char tryCount = 0;

        std::string fullPath = "";
        if(subPath.has_value())
        {
            fullPath += subPath.value() + "/";
        }
        fullPath += catalogName;
    
        bool isPresent = isDirectoryValid(fullPath);
        setCommandHandler(std::make_unique<RemoveCommand>(catalogName, targetType::directory, subPath));
        
        while(isPresent && tryCount < 3)
        {
            comHandler_->execute();
            isPresent = isDirectoryValid(fullPath);
            ++tryCount;
        }
        return !isPresent;
    }

    bool DirectoryManager::isPathGood(std::string path)
    {
        // Check for illeagal characters (Linux-wise)
        // The backward slash
        if(path.find_first_of('\\') != std::string::npos)
        {
            std::cout << "Found invalid character \"\\\"\n";
            return false;
        }
        return true;
    }


    bool DirectoryManager::isDirectoryValid(std::string path)
    {
        if(!isPathGood(path)) return false;

        setCommandHandler(std::make_unique<verifyCommand>(path));
        return comHandler_->execute();
    }
} // namespace Utilities::Workspace
