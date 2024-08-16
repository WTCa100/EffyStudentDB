#pragma once

#include <iostream>

#include "../commandHandler.hpp"

namespace Utilities::Command
{
    class RemoveCommand : public CommandHandler
    {
    public:
        RemoveCommand(std::string target, std::string targetType);
        ~RemoveCommand();
    };
    
} // namespace Utilities::Command

