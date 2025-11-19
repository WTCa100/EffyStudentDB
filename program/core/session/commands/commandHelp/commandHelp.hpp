#pragma once

#include "../ICommand.hpp"

#include <memory>

namespace Core::Commands
{
    class CommandHelp : public ICommand
    {
      public:
        bool exec() override;

        std::string name() const override { return "help"; }

        CommandHelp(std::shared_ptr<Utilities::Logger> logger):
            ICommand(logger) {};
        ~CommandHelp() = default;
    };

}  // namespace Core::Commands
