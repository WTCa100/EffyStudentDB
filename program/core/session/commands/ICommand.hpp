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
    };
}  // namespace Core::Commands
