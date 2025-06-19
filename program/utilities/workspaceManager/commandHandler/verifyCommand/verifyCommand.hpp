#pragma once

#include "../commandHandler.hpp"

#include <filesystem>
#include <optional>

namespace Utilities::Command
{
    class verifyCommand : public CommandHandler
    {
      private:
        std::string target_;
        std::optional<std::filesystem::path> subPath_;

      public:
        verifyCommand(std::string target, std::optional<std::filesystem::path> subPath = std::nullopt):
            target_(target),
            subPath_(subPath)
        {}

        ~verifyCommand() = default;
        bool execute() override;
    };

}  // namespace Utilities::Command
