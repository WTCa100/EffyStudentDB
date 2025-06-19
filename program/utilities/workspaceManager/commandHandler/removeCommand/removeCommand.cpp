#include "removeCommand.hpp"

namespace Utilities::Command
{

    bool RemoveCommand::execute()
    {
        std::string fullPath = assambleFullPath(target_, subPath_, std::filesystem::current_path().string());

        switch (type_)
        {
            case targetType::directory : std::filesystem::remove_all(fullPath); return !std::filesystem::exists(fullPath);
            case targetType::file : std::filesystem::remove(fullPath); return !std::filesystem::exists(fullPath);
            default : return false;
        }
    }
}  // namespace Utilities::Command
