#include "dirManager.hpp"

#include "../commandHandler/createCommand/createCommand.hpp"
#include "../commandHandler/removeCommand/removeCommand.hpp"
#include "../commandHandler/verifyCommand/verifyCommand.hpp"

#include <cstring>
#include <iostream>

using namespace Utilities::Command;

namespace Utilities::Workspace
{

    DirectoryManager::DirectoryManager(std::string rootPath):
        appRoot_(std::filesystem::path(rootPath)),
        cwd_(appRoot_)
    {
        std::cout << "DirManager :ctor: appRoot_=" << appRoot_.string() << " (from specialized ctor)\n";
    }

    DirectoryManager::DirectoryManager():
        DirectoryManager(std::filesystem::current_path())
    {}

    bool DirectoryManager::createDirectory(std::string directoryName, std::optional<std::string> subPath)
    {
        setCommandHandler(std::make_unique<CreateCommand>(directoryName, targetType::directory, subPath));
        return comHandler_->execute();
    }

    bool DirectoryManager::deleteDirectory(std::string directoryName, std::optional<std::string> subPath)
    {
        setCommandHandler(std::make_unique<RemoveCommand>(directoryName, targetType::directory, subPath));
        if (!comHandler_->execute()) { return false; }
        return !exist(directoryName, subPath);
    }

    bool DirectoryManager::isPathGood(std::string path)
    {
        // Check for illeagal characters (Linux-wise)
        // The backward slash
        if (path.find_first_of('\\') != std::string::npos)
        {
            std::cout << "Found invalid character \"\\\"\n";
            return false;
        }
        return true;
    }

    bool DirectoryManager::exist(std::string directoryName, std::optional<std::string> subPath)
    {
        if (!isPathGood(directoryName)) return false;

        setCommandHandler(std::make_unique<verifyCommand>(directoryName, subPath));
        return comHandler_->execute();
    }
}  // namespace Utilities::Workspace
