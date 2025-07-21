#pragma once

#include "../../../utilities/common/stringManip.hpp"

#include <set>
#include <string>
#include <vector>

namespace Core
{

    namespace ActionType
    {
        namespace Short
        {
            const std::string actionHelp = "HELP";
            const std::string actionShow = "SHOW";
            const std::string actionList = "LIST";
            const std::string actionExit = "EXIT";
        }  // namespace Short

        namespace Direct
        {

            const std::string actionAdd    = "ADD";
            const std::string actionRemove = "REMOVE";
            const std::string actionFind   = "FIND";
            const std::string actionEdit   = "EDIT";
        }  // namespace Direct

        namespace Indirect
        {
            const std::string actionAssign = "ASSIGN";
            const std::string actionDrop   = "DROP";
        }  // namespace Indirect

    }  // namespace ActionType

    enum class ActionValue
    {
        command          = 0,
        target           = 1,
        additionalValues = 2
    };

    static std::set<std::string> validIndirectCommands_{ ActionType::Indirect::actionAssign, ActionType::Indirect::actionDrop };
    static std::set<std::string> validDirectCommands_{ ActionType::Direct::actionAdd, ActionType::Direct::actionEdit,
        ActionType::Direct::actionFind, ActionType::Direct::actionRemove };
    static std::set<std::string> validShortCommands_{ ActionType::Short::actionExit, ActionType::Short::actionHelp,
        ActionType::Short::actionList, ActionType::Short::actionShow };

    struct Action
    {
        std::vector<std::string> tokens_;

        Action():
            tokens_({})
        {}

        Action(const std::string& input, const char& delim = ' '):
            tokens_(Utilities::Common::tokenize(input, delim))
        {}

        ~Action() = default;
        std::string getCommand() const;
        std::string getTarget() const;
        std::vector<std::string> getAdditionalValues() const;

        static bool isCommandIndirect(const std::string commandToken) { return validIndirectCommands_.contains(commandToken); }

        static bool isCommandDirect(const std::string commandToken) { return validDirectCommands_.contains(commandToken); }

        static bool isCommandShort(const std::string commandToken) { return validShortCommands_.contains(commandToken); }

        static bool isCommandValid(const std::string commandToken)
        {
            return isCommandDirect(commandToken) || isCommandIndirect(commandToken) || isCommandShort(commandToken);
        }
    };

}  // namespace Core

