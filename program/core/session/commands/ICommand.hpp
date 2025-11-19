#pragma once

#include "../../../utilities/logger/logger.hpp"

namespace Core::Commands
{
    class ICommand
    {
      protected:
        std::shared_ptr<Utilities::Logger> logger_;

      public:
        ICommand(std::shared_ptr<Utilities::Logger> logger):
            logger_(logger) {};
        virtual bool exec()              = 0;
        virtual ~ICommand()              = default;
        virtual std::string name() const = 0;
        // TODO: Maybe add "virtual std::string successMessage() const = 0; and display that instead - to make it more pretty and
        // readable for the user."
        // TODO: Also add "virtual std::string usageMessage() const = 0; to inform user how to use given command."
    };
}  // namespace Core::Commands
