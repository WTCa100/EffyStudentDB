#include "removeCommand.hpp"


namespace Utilities::Command
{

    bool RemoveCommand::execute()
    {
        std::string fullPath = assambleFullPath(target_, subPath_, std::filesystem::current_path().string());

        std::cout << "Attempting to remove: " << fullPath << "\n";

        switch (type_)
        {
        case targetType::directory:
            std::filesystem::remove_all(fullPath);
            break;
        case targetType::file:
            std::filesystem::remove(fullPath);
        default:
            break;
        }
    }    
} // namespace Utilities::Command
