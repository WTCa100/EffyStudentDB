#include "CommandFactory.hpp"

#include "../../../utilities/common/stringManip.hpp"
#include "../../../utilities/inputHandler/inputHandler.hpp"
#include "../../../types/entryFactory.hpp"
#include "commandAdd/commandAdd.hpp"
#include "commandExit/commandExit.hpp"
#include "commandHelp/commandHelp.hpp"
#include "commandShow/commandShow.hpp"
#include "commandDrop/commandDrop.hpp"
#include "commandFind/commandFind.hpp"
#include "commandOpen/commandOpen.hpp"
#include "commandClose/commandClose.hpp"
#include "commandEdit/commandEdit.hpp"
#include "commandAssign/commandAssign.hpp"
#include "commandRemove/commandRemove.hpp"
#include "commandList/commandList.hpp"
namespace Core::Commands
{

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
        // First check if its a simple command and if its valid.
        if (commandType == "HELP" || commandType == "EXIT" || commandType == "LIST")
        {
            LOG((*logger_), "Command is a simple command.");
            return true;
        }

        if (commandSize < 2)
        {
            LOG((*logger_), "Non-simple command of invalid size of 1");
            return false;
        }
        // Second check if its a normal command and if its valid.
        if (commandType == "ADD" || commandType == "SHOW" || commandType == "FIND" || commandType == "REMOVE" || commandType == "EDIT")
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

        // TODO change commands so that they are aggregated into different types
        if (commandType == "OPEN" || commandType == "CLOSE")
        {
            LOG((*logger_), "Command is course management.");
            try
            {
                [[maybe_unused]] const uint16_t targetCourse = static_cast<uint16_t>(std::stoul(tokenizedCommand.at(uint8_t(CommandTokensPosition::target))));
                return true;
            }
            catch(const std::exception& e)
            {
                std::cout << "Error: " << e.what() << '\n';
                return false;
            }
        }

        // Thrice check if its a link command and if its valid.
        if (commandSize < 3)
        {
            LOG((*logger_), "Link command of invalid size. (Expected 3 got ", commandSize, ")");
            return false;
        }

        if (commandType == "ASSIGN" || commandType == "DROP")
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
        if (commandType == "EXIT")  // Change it later for tokenization handling
        {
            return std::make_unique<CommandExit>(logger_);
        }
        else if (commandType == "HELP") { return std::make_unique<CommandHelp>(logger_); }
        else if (commandType == "LIST") { return std::make_unique<CommandList>(logger_, sAdapter_);}

        const std::string commandTarget = tokens.at(uint8_t(CommandTokensPosition::target));
        if (commandType == "ADD")
        {
            std::shared_ptr<Entry> newEntry = Core::Types::EntryFactory::getEntry(commandTarget);
            newEntry->userConstruct();
            return std::make_unique<CommandAdd>(logger_, sAdapter_, sessionData_, newEntry);
        }

        if (commandType == "EDIT")
        {
            std::shared_ptr<Entry> editedEntryFilteredValues = Core::Types::EntryFactory::getEntry(commandTarget);
            std::unordered_map<std::string, std::string> filterInput = editedEntryFilteredValues->userConstruct(false);
            return std::make_unique<CommandEdit>(logger_, sAdapter_, sessionData_, editedEntryFilteredValues, filterInput);
        }

        if (commandType == "FIND")
        {
            std::shared_ptr<Entry> targetEntry = Core::Types::EntryFactory::getEntry(commandTarget);
            std::unordered_map<std::string, std::string> filterInput = targetEntry->userConstruct(false);
            return std::make_unique<CommandFind>(logger_, sAdapter_, displayHelper_, filterInput, targetEntry);
        }

        if (commandType == "REMOVE")
        {
            std::shared_ptr<Entry> targetEntry = Core::Types::EntryFactory::getEntry(commandTarget);
            return std::make_unique<CommandRemove>(logger_, sAdapter_, sessionData_, targetEntry);
        }

        if (commandType == "SHOW")
        {
            return std::make_unique<CommandShow>(logger_, sessionData_, sAdapter_, displayHelper_, commandTarget);
        }

        // Assign and Drop can be re-assigned into a one block
        if (commandType == "ASSIGN")
        {
            // No exception ensured by previous validation.
            const uint16_t targetCourseId  = uint16_t(std::stoul(tokens.at(uint8_t(CommandTokensPosition::linkTarget))));
            const uint16_t targetStudentId = uint16_t(std::stoul(tokens.at(uint8_t(CommandTokensPosition::linkInvoker))));

            if (!sessionData_->isPresent(targetCourseId, g_tableCourses))
            {
                LOG((*logger_), "No such course with id: ", targetCourseId);
                std::cout << "Could not assign student to course. " << targetCourseId << " is not a valid course ID!\n";
                return nullptr;
            }

            if (!sessionData_->isPresent(targetStudentId, g_tableStudents))
            {
                LOG((*logger_), "No such student with id: ", targetStudentId);
                std::cout << "Could not assign student to course. " << targetStudentId << " is not a valid student ID!\n";
                return nullptr;
            }
            return std::make_unique<CommandAssign>(logger_, sAdapter_, targetCourseId, targetStudentId);
        }

        if (commandType == "DROP")
        {
            // No exception ensured by previous validation.
            const uint16_t targetCourseId  = uint16_t(std::stoul(tokens.at(uint8_t(CommandTokensPosition::linkTarget))));
            const uint16_t targetStudentId = uint16_t(std::stoul(tokens.at(uint8_t(CommandTokensPosition::linkInvoker))));

            if (!sessionData_->isPresent(targetCourseId, g_tableCourses))
            {
                LOG((*logger_), "No such course with id: ", targetCourseId);
                std::cout << "Could not drop student from course. " << targetCourseId << " is not a valid course ID!\n";
                return nullptr;
            }

            if (!sessionData_->isPresent(targetStudentId, g_tableStudents))
            {
                LOG((*logger_), "No such student with id: ", targetStudentId);
                std::cout << "Could not drop student from course. " << targetStudentId << " is not a valid student ID!\n";
                return nullptr;
            }
            return std::make_unique<CommandDrop>(logger_, sAdapter_, targetCourseId, targetStudentId);
        }

        if (commandType == "OPEN")
        {
            const uint16_t targetCourseId = uint16_t(std::stoul(tokens.at(uint8_t(CommandTokensPosition::target))));
            if(!sessionData_->isPresent(targetCourseId, g_tableCourses))
            {
                LOG((*logger_), "No such course with id: ", targetCourseId);
                std::cout << "Could not open course. " << targetCourseId << " is not a valid course ID!\n";
                return nullptr;
            }
            return std::make_unique<CommandOpen>(logger_, sessionData_, sAdapter_, targetCourseId);
        }

        if (commandType == "CLOSE")
        {
            const uint16_t targetCourseId = uint16_t(std::stoul(tokens.at(uint8_t(CommandTokensPosition::target))));
            if(!sessionData_->isPresent(targetCourseId, g_tableCourses))
            {
                LOG((*logger_), "No such course with id: ", targetCourseId);
                std::cout << "Could not close course. " << targetCourseId << " is not a valid course ID!\n";
                return nullptr;
            }
            return std::make_unique<CommandClose>(logger_, sessionData_, sAdapter_, targetCourseId);
        }
        return nullptr;
    }
}  // namespace Core::Commands
