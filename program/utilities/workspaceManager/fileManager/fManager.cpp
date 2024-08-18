#include "fManager.hpp"

#include "../commandHandler/createCommand/createCommand.hpp"
#include "../commandHandler/removeCommand/removeCommand.hpp"
#include "../commandHandler/verifyCommand/verifyCommand.hpp"


using namespace Utilities::Command;
namespace Utilities::Workspace
{
    FileManager::FileManager(std::string rootPath) : appRoot_(std::filesystem::path(rootPath)), cwd_(appRoot_)
    {
        std::cout << "fManager :ctor: appRoot_ = " << appRoot_.string() << " (from specialized ctor)\n"; 
    }

    FileManager::FileManager() : appRoot_(std::filesystem::current_path()), cwd_(appRoot_)
    {
        std::cout << "fManager :ctor: appRoot_ " << appRoot_.string() << "\n";
    }

    bool FileManager::createFile(std::string fileName, std::optional<std::filesystem::path> subPath)
    {
        setCommandHandler(std::make_unique<CreateCommand>(fileName, targetType::file, subPath));
        return comHandler_->execute();
    }

    bool FileManager::deleteFile(std::string fileName, std::optional<std::filesystem::path> subPath)
    {
        setCommandHandler(std::make_unique<RemoveCommand>(fileName, targetType::file, subPath));
        return comHandler_->execute();
    }

    bool FileManager::exist(std::string fileName, std::optional<std::filesystem::path> subPath)
    {
        setCommandHandler(std::make_unique<verifyCommand>(fileName, subPath));
        return comHandler_->execute();
    }

} // namespace Utilities::Workspace
