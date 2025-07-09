#include "fManager.hpp"

#include "../commandHandler/createCommand/createCommand.hpp"
#include "../commandHandler/removeCommand/removeCommand.hpp"
#include "../commandHandler/verifyCommand/verifyCommand.hpp"

using namespace Utilities::Command;

namespace Utilities::Workspace
{
    FileManager::FileManager(std::string rootPath):
        appRoot_(std::filesystem::path(rootPath)),
        cwd_(appRoot_)
    {
        std::cout << "fManager :ctor: appRoot_ = " << appRoot_.string() << " (from specialized ctor)\n";
    }

    FileManager::FileManager():
        appRoot_(std::filesystem::current_path()),
        cwd_(appRoot_)
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

    std::unique_ptr<std::fstream> FileManager::getFile(std::string fileName, std::optional<std::filesystem::path> subPath)
    {
        if (!exist(fileName, subPath)) { return nullptr; }
        std::unique_ptr<std::fstream> ptrRet = std::make_unique<std::fstream>();
        ptrRet->open(CommandHandler::assambleFullPath(fileName, subPath));
        return ptrRet;
    }

    std::vector<std::string> FileManager::getFileContent(std::string fileName, std::optional<std::filesystem::path> subPath)
    {
        std::vector<std::string> content;
        std::ifstream fPtr;

        if (subPath.has_value())
        {
            std::filesystem::path& subjectPath = subPath.value();
            if (subjectPath.has_filename())
            {
                std::cout << "subPath already has a filename. Replacing it with: " << subjectPath.filename() << std::endl;
                fPtr.open(subjectPath);
            }
            else { fPtr.open(subjectPath.string() + "/" + fileName); }
        }
        else { fPtr.open(fileName); }

        std::string line;

        while (std::getline(fPtr, line))
        {
            std::cout << line << "\n";
            content.push_back(line);
        }
        return content;
    }

    bool FileManager::exist(std::string fileName, std::optional<std::filesystem::path> subPath)
    {
        setCommandHandler(std::make_unique<verifyCommand>(fileName, subPath));
        return comHandler_->execute();
    }

}  // namespace Utilities::Workspace
