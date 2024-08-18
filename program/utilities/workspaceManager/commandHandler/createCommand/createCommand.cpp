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
            std::cout << "Creating directory: " << target_ << "\n";
            if(subPath_.has_value())
            {
                std::cout << "Sub-path = " << subPath_.value() << "\n";
            }
            std::filesystem::create_directory(fullPath);
            return true;
        }
        case targetType::file:
        {
            std::cout << "Creating file: " << target_ << "\n";
            if(subPath_.has_value())
            {
                std::cout << "Sub-path = " << subPath_.value() << "\n";
            }
            std::ofstream fileOut(fullPath);
            return true;
        }
        default:
            return false;
        }
    }
} // namespace Utilities::Command
