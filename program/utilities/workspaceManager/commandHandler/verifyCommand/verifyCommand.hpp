#pragma once

#include "../commandHandler.hpp"

#include <optional>
#include <filesystem>

namespace Utilities::Command
{
    class verifyCommand : public CommandHandler 
    {
    private:
        std::string target_;
        std::optional<std::filesystem::path> subPath_;
        std::string prepareCommand() override;
    public:
        verifyCommand(std::string target, std::optional<std::filesystem::path> subPath = std::nullopt) : target_(target), subPath_(subPath) {}
        ~verifyCommand() = default;
    };

} // namespace Utilities::Command
