#include "CommandFactory.hpp"

#include "../../../utilities/common/stringManip.hpp"
#include "../../../utilities/inputHandler/inputHandler.hpp"
#include "commandAdd/commandAdd.hpp"
#include "commandAssign/commandAssign.hpp"
#include "commandExit/commandExit.hpp"
#include "commandHelp/commandHelp.hpp"
#include "commandShow/commandShow.hpp"

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
        if (commandType == "HELP" || commandType == "EXIT")
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
        if (commandType == "ADD" || commandType == "SHOW")
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

        // Thrice check if its a link command and if its valid.
        if (commandSize < 3)
        {
            LOG((*logger_), "Link command of invalid size. (Expected 3 got ", commandSize, ")");
            return false;
        }

        if (commandType == "ASSIGN")
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

    std::shared_ptr<Entry> CommandFactory::makeConcreteType(const std::string& tableName)
    {
        std::shared_ptr<Entry> tmp;
        // Extract concrete type
        if (tableName == g_tableSchools) { tmp = std::make_shared<School>(); }
        else if (tableName == g_tableStudents) { tmp = std::make_shared<Student>(); }
        else if (tableName == g_tableSubjects) { tmp = std::make_shared<Subject>(); }
        else if (tableName == g_tableCourses) { tmp = std::make_shared<Course>(); }
        else if (tableName == g_tableGrades) { tmp = std::make_shared<Grade>(); }
        else if (tableName == g_tableCourseSubjectWeight) { tmp = std::make_shared<CourseSubjectWeight>(); }
        else { tmp = std::make_shared<Srequest>(); }
        return tmp;
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

        const std::string commandTarget = tokens.at(uint8_t(CommandTokensPosition::target));
        if (commandType == "ADD")
        {
            std::shared_ptr<Entry> newEntry = makeConcreteType(commandTarget);
            newEntry->userConstruct();
            return std::make_unique<CommandAdd>(logger_, sAdapter_, sessionData_, newEntry);
        }

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

        if (commandType == "SHOW")
        {
            return std::make_unique<CommandShow>(logger_, sessionData_, sAdapter_, displayHelper_, commandTarget);
        }
        return nullptr;
    }
}  // namespace Core::Commands
