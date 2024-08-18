#pragma once

#include <iostream>
#include <filesystem>
#include <optional>

#include "../commandHandler.hpp"

namespace Utilities::Command
{
    /**
     * @brief Takes care of deleting either files or directories.
     * @warning This method may result in permament deletion of a given target.
     * This cannot be undone, so use with caution.
     */
    class RemoveCommand : public CommandHandler
    {
    private:
        std::string target_;
        targetType type_;
        std::optional<std::filesystem::path> subPath_;
    public:
        RemoveCommand(std::string target, targetType type, std::optional<std::filesystem::path> subPath = std::nullopt) : target_(target), type_(type), subPath_(subPath) {}
        ~RemoveCommand() = default;
        bool execute() override;


    };
    
} // namespace Utilities::Command

