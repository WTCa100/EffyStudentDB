#pragma once

#include <iostream>
#include <optional>
#include <filesystem>

#include "../commandHandler.hpp"

namespace Utilities::Command
{
    class CreateCommand : public CommandHandler
    {
    private:
        std::string target_;
        targetType type_;
        std::optional<std::filesystem::path> subPath_;
        std::string prepareCommand() override;
    public:
        CreateCommand(std::string target, targetType type, std::optional<std::filesystem::path> subPath = std::nullopt) : target_(target), type_(type), subPath_(subPath) {}
        virtual ~CreateCommand() = default;
    };    
} // namespace Utilities::Command


