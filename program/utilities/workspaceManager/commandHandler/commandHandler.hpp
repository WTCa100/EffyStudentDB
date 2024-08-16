#pragma once

#include <iostream>


namespace Utilities::Command
{
    enum class targetType
    {
        file      = 0,
        directory = 1
    };

    class CommandHandler
    {
    protected:
        std::string command_;
        virtual std::string prepareCommand() = 0;
    public:
        CommandHandler() : command_("") {}
        bool execute();
        virtual ~CommandHandler() {}
        // static bool isDirectoryGood(); // Verify command
    };
} // namespace Utilities::Command