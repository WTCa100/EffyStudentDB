#include "commandHandler.hpp"

namespace Utilities::Command
{
    bool CommandHandler::execute()
    {
        command_ = prepareCommand();
        std::cout << "Running shell command: " << command_ << "\n";
        return system(command_.c_str()) == 0 ? true : false;
    }
} // namespace Utilities::Command
