#include "createCommand.hpp"

#include <filesystem>

namespace Utilities::Command
{
    std::string CreateCommand::prepareCommand()
    {
        std::string path = std::filesystem::current_path().string() + "/";
        if(subPath_.has_value())
        {
            path += subPath_.value().string() + "/";
        }
        path += target_;

        switch (type_)
        {
            case targetType::directory:
            {
                return "mkdir --verbose " + path;
            }
            case targetType::file:
            {
                return "touch " + path;
            }
            default:
                std::cout << "Uknown target type!\n";
                return "";
        }
    }
} // namespace Utilities::Command
