#include "createCommand.hpp"

#include <filesystem>
#include <fstream>

namespace Utilities::Command
{
    bool CreateCommand::execute()
    {
        std::string fullPath = assambleFullPath(target_, subPath_, std::filesystem::current_path().string());

        switch (type_)
        {
        case targetType::directory:
        {
            std::filesystem::create_directory(fullPath);
            return true;
        }
        case targetType::file:
        {
            std::ofstream fileOut(fullPath);
            return true;
        }
        default:
            return false;
        }
    }
} // namespace Utilities::Command
