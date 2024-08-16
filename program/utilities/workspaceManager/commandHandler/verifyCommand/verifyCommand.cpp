#include "verifyCommand.hpp"



namespace Utilities::Command
{
    std::string verifyCommand::prepareCommand()
    {
        std::string path = std::filesystem::path().string();
        if(subPath_.has_value())
        {
            path += subPath_.value().string() + "/";
        }
        path += target_;
        return "[ -d " + path + " ]";
    }

} // namespace Utilities::Command
