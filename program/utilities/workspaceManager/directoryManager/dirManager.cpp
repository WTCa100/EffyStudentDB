#include <iostream>
#include <cstring>

#include "dirManager.hpp"

#include "../commandHandler/createCommand/createCommand.hpp"
#include "../commandHandler/verifyCommand/verifyCommand.hpp"

using namespace Utilities::Command;
namespace Utilities::Workspace
{

    DirectoryManager::DirectoryManager(std::string rootPath) : appRoot_(std::filesystem::path(rootPath))
    {
        std::cout << "DirManager :ctor: appRoot_=" << appRoot_ << " (from specialized ctor)\n";
    }

    DirectoryManager::DirectoryManager() : appRoot_(std::filesystem::current_path())
    {
        std::cout << "DirManager :ctor: appRoot_= " << appRoot_.c_str() << "\n";
    }

    bool DirectoryManager::createDirectory(std::string catalogName)
    {
        setCommandHandler(std::make_unique<CreateCommand>(catalogName, targetType::directory));
        return comHandler_->execute();
    }

    bool DirectoryManager::removeDirectory(std::string catalogName)
    {
        unsigned char tryCount = 0;
        bool isPresent = true;
        do
        {
            // First check the directory - perhaps it doesn't exist!
            // isPresent = isDirectoryValid();

        } while(tryCount <= 3 || isPresent);
    }

    bool DirectoryManager::isDirectoryValid(std::string path)
    {
        // Check for illeagal characters (Linux-wise)
        // The backward slash
        if(path.find_first_of('\\') != std::string::npos)
        {
            std::cout << "Found invalid character \"\\\"\n";
            return false;
        }

        setCommandHandler(std::make_unique<verifyCommand>(path));
        return comHandler_->execute();
    }

    DirectoryManager::~DirectoryManager() {}
} // namespace Utilities::Workspace
