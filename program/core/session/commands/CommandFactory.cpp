#include "CommandFactory.hpp"

#include "../../../types/entryFactory.hpp"
#include "../../../utilities/common/stringManip.hpp"
#include "../../../utilities/inputHandler/inputHandler.hpp"
#include "commandAdd/commandAdd.hpp"
#include "commandAssign/commandAssign.hpp"
#include "commandClose/commandClose.hpp"
#include "commandDrop/commandDrop.hpp"
#include "commandEdit/commandEdit.hpp"
#include "commandExit/commandExit.hpp"
#include "commandFind/commandFind.hpp"
#include "commandHelp/commandHelp.hpp"
#include "commandList/commandList.hpp"
#include "commandOpen/commandOpen.hpp"
#include "commandRemove/commandRemove.hpp"
#include "commandShow/commandShow.hpp"

namespace Core::Commands
{
    namespace
    {
        // Command type constants
        constexpr std::string_view CMD_EXIT = "EXIT";
        constexpr std::string_view CMD_HELP = "HELP";
        constexpr std::string_view CMD_LIST = "LIST";
        constexpr std::string_view CMD_ADD = "ADD";
        constexpr std::string_view CMD_SHOW = "SHOW";
        constexpr std::string_view CMD_FIND = "FIND";
        constexpr std::string_view CMD_REMOVE = "REMOVE";
        constexpr std::string_view CMD_EDIT = "EDIT";
        constexpr std::string_view CMD_OPEN = "OPEN";
        constexpr std::string_view CMD_CLOSE = "CLOSE";
        constexpr std::string_view CMD_ASSIGN = "ASSIGN";
        constexpr std::string_view CMD_DROP = "DROP";

        // Helper to check if command type matches
        bool isCommandType(std::string_view cmd, std::string_view type)
        {
            return cmd == type;
        }

        // Helper to check if command type is one of several
        bool isCommandTypeOneOf(std::string_view cmd,
            std::initializer_list<std::string_view> types)
        {
            return std::any_of(types.begin(), types.end(),
                [cmd](std::string_view type) { return cmd == type; });
        }
    }  // namespace

    bool CommandFactory::validateCommand(const std::vector<std::string>& tokenizedCommand)
    {
        LOG((*logger_), "Validating command.");
        if (tokenizedCommand.empty())
        {
            LOG((*logger_), "Tokens are empty!");
            return false;
        }

        const size_t commandSize      = tokenizedCommand.size();
        const std::string commandType = tokenizedCommand.at(uint8_t(CommandTokensPosition::type));

        // Check simple commands (no additional arguments required)
        if (isCommandTypeOneOf(commandType, {CMD_HELP, CMD_EXIT, CMD_LIST}))
        {
            LOG((*logger_), "Command is a simple command.");
            return true;
        }

        if (commandSize < 2)
        {
            LOG((*logger_), "Non-simple command of invalid size of 1");
            return false;
        }

        // Check normal commands (require a table target)
        if (isCommandTypeOneOf(commandType, {CMD_ADD, CMD_SHOW, CMD_FIND, CMD_REMOVE, CMD_EDIT}))
        {
            LOG((*logger_), "Command is a normal command.");
            const std::string commandTarget = tokenizedCommand.at(uint8_t(CommandTokensPosition::target));
            if (!sessionData_->tableExists(commandTarget))
            {
                LOG((*logger_), "Session Data does not contain table with name =", commandTarget);
                return false;
            }
            return true;
        }

        // Check course management commands (require numeric course ID)
        if (isCommandTypeOneOf(commandType, {CMD_OPEN, CMD_CLOSE}))
        {
            LOG((*logger_), "Command is course management.");
            try
            {
                [[maybe_unused]] const uint16_t targetCourse =
                    static_cast<uint16_t>(std::stoul(tokenizedCommand.at(uint8_t(CommandTokensPosition::target))));
                return true;
            }
            catch (const std::exception& e)
            {
                std::cout << "Error: " << e.what() << '\n';
                return false;
            }
        }

        // Check link commands (require 3 tokens)
        if (commandSize < 3)
        {
            LOG((*logger_), "Link command of invalid size. (Expected 3 got ", commandSize, ")");
            return false;
        }

        // Check linking commands (ASSIGN, DROP)
        if (isCommandTypeOneOf(commandType, {CMD_ASSIGN, CMD_DROP}))
        {
            LOG((*logger_), "Command is linking command.");
            const std::string linkTarget  = tokenizedCommand.at(uint8_t(CommandTokensPosition::linkTarget));
            const std::string linkInvoker = tokenizedCommand.at(uint8_t(CommandTokensPosition::linkInvoker));
            if (!Utilities::InputHandler::isNumber(linkTarget) || !Utilities::InputHandler::isNumber(linkInvoker))
            {
                LOG((*logger_), "Values are not integers! linkTarget=", linkTarget, " linkInvoker=", linkInvoker);
                return false;
            }
            return true;
        }
        return false;
    }

    bool CommandFactory::validateCourseId(uint16_t courseId, const std::string& operation)
    {
        if (!sessionData_->isPresent(courseId, g_tableCourses))
        {
            LOG((*logger_), "No such course with id: ", courseId);
            std::cout << "Could not " << operation << " course. " << courseId << " is not a valid course ID!\n";
            return false;
        }
        return true;
    }

    bool CommandFactory::validateStudentId(uint16_t studentId, const std::string& operation)
    {
        if (!sessionData_->isPresent(studentId, g_tableStudents))
        {
            LOG((*logger_), "No such student with id: ", studentId);
            std::cout << "Could not " << operation << " student. " << studentId << " is not a valid student ID!\n";
            return false;
        }
        return true;
    }

    std::unique_ptr<ICommand> CommandFactory::makeCommand(const std::string& rawCommand)
    {
        LOG((*logger_), "Making command from: ", rawCommand);
        std::vector<std::string> tokens = Utilities::Common::tokenize(rawCommand, ' ');

        if (!validateCommand(tokens))
        {
            LOG((*logger_), "Invalid command");
            return nullptr;
        }

        const std::string commandType = tokens.at(uint8_t(CommandTokensPosition::type));

        // Simple commands
        if (isCommandType(commandType, CMD_EXIT))
            return std::make_unique<CommandExit>(logger_);

        if (isCommandType(commandType, CMD_HELP))
            return std::make_unique<CommandHelp>(logger_);

        if (isCommandType(commandType, CMD_LIST))
            return std::make_unique<CommandList>(logger_, sAdapter_);

        const std::string commandTarget = tokens.at(uint8_t(CommandTokensPosition::target));

        // Normal table-based commands
        if (isCommandType(commandType, CMD_ADD))
        {
            std::shared_ptr<Entry> newEntry = Core::Types::EntryFactory::getEntry(commandTarget);
            newEntry->userConstruct();
            return std::make_unique<CommandAdd>(logger_, sAdapter_, sessionData_, newEntry);
        }

        if (isCommandType(commandType, CMD_EDIT))
        {
            std::shared_ptr<Entry> editedEntryFilteredValues         = Core::Types::EntryFactory::getEntry(commandTarget);
            std::unordered_map<std::string, std::string> filterInput = editedEntryFilteredValues->userConstruct(false);
            return std::make_unique<CommandEdit>(logger_, sAdapter_, sessionData_, editedEntryFilteredValues, filterInput);
        }

        if (isCommandType(commandType, CMD_FIND))
        {
            std::shared_ptr<Entry> targetEntry                       = Core::Types::EntryFactory::getEntry(commandTarget);
            std::unordered_map<std::string, std::string> filterInput = targetEntry->userConstruct(false);
            return std::make_unique<CommandFind>(logger_, sAdapter_, displayHelper_, filterInput, targetEntry);
        }

        if (isCommandType(commandType, CMD_REMOVE))
        {
            std::shared_ptr<Entry> targetEntry = Core::Types::EntryFactory::getEntry(commandTarget);
            return std::make_unique<CommandRemove>(logger_, sAdapter_, sessionData_, targetEntry);
        }

        if (isCommandType(commandType, CMD_SHOW))
        {
            return std::make_unique<CommandShow>(logger_, sessionData_, sAdapter_, displayHelper_, commandTarget);
        }

        // Course management commands
        const uint16_t courseId = uint16_t(std::stoul(commandTarget));

        if (isCommandType(commandType, CMD_OPEN))
        {
            if (!validateCourseId(courseId, "open"))
                return nullptr;
            return std::make_unique<CommandOpen>(logger_, sessionData_, sAdapter_, courseId);
        }

        if (isCommandType(commandType, CMD_CLOSE))
        {
            if (!validateCourseId(courseId, "close"))
                return nullptr;
            return std::make_unique<CommandClose>(logger_, sessionData_, sAdapter_, courseId);
        }

        // Link-based commands (ASSIGN, DROP)
        const uint16_t targetCourseId  = uint16_t(std::stoul(tokens.at(uint8_t(CommandTokensPosition::linkTarget))));
        const uint16_t targetStudentId = uint16_t(std::stoul(tokens.at(uint8_t(CommandTokensPosition::linkInvoker))));

        if (isCommandType(commandType, CMD_ASSIGN))
        {
            if (!validateCourseId(targetCourseId, "assign student to") ||
                !validateStudentId(targetStudentId, "assign to course"))
                return nullptr;
            return std::make_unique<CommandAssign>(logger_, sAdapter_, targetCourseId, targetStudentId);
        }

        if (isCommandType(commandType, CMD_DROP))
        {
            if (!validateCourseId(targetCourseId, "drop student from") ||
                !validateStudentId(targetStudentId, "drop from course"))
                return nullptr;
            return std::make_unique<CommandDrop>(logger_, sAdapter_, targetCourseId, targetStudentId);
        }

        return nullptr;
    }
}  // namespace Core::Commands
