#include "CommandFactory.hpp"

#include "commandExit/commandExit.hpp"
#include "commandHelp/commandHelp.hpp"
#include "commandAdd/commandAdd.hpp"
#include "commandShow/commandShow.hpp"
#include "../../../utilities/common/stringManip.hpp"


namespace Core::Commands
{

    bool CommandFactory::validateCommand(const std::vector<std::string>& tokenizedCommand)
    {
        LOG((*logger_), "Validating command.");
        if(tokenizedCommand.empty())
        {
            LOG((*logger_), "Tokens are empty!");
            return false;
        }

        const std::string commandType = tokenizedCommand.at(uint8_t(CommandTokensPosition::type));
        // First check if its a simple command and if its valid.
        if(commandType == "HELP" ||
           commandType == "EXIT")
        {
            LOG((*logger_), "Command is a simple command.");
            return true;
        }

        // Second check if its a normal command and if its valid.
        if(commandType == "ADD" ||
           commandType == "SHOW")
        {
            LOG((*logger_), "Command is a normal command.");
            if(tokenizedCommand.size() < 2)
            {
                LOG((*logger_), "Normal command of invalid size of 1");
                return false;
            }

            const std::string commandTarget = tokenizedCommand.at(uint8_t(CommandTokensPosition::target));
            if(!sessionData_->tableExists(commandTarget))
            {
                LOG((*logger_), "Session Data does not contain table with name =", commandTarget);
                return false;
            }

            return true;
        }

        // Thrice check if its a link command and if its valid.
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
        
        if(!validateCommand(tokens))
        {
            LOG((*logger_), "Invalid command");
            return nullptr;
        }

        const std::string commandType = tokens.at(uint8_t(CommandTokensPosition::type));
        if(commandType == "EXIT") // Change it later for tokenization handling
        {
            return std::make_unique<CommandExit>(logger_);
        }
        else if(commandType == "HELP")
        {
            return std::make_unique<CommandHelp>(logger_);
        }
        
        const std::string commandTarget = tokens.at(uint8_t(CommandTokensPosition::target));
        if(commandType == "ADD")
        {
            std::shared_ptr<Entry> newEntry = makeConcreteType(commandTarget);
            newEntry->userConstruct();
            return std::make_unique<CommandAdd>(logger_, sAdapter_, sessionData_, newEntry);
        }

        if(commandType == "SHOW")
        {
            return std::make_unique<CommandShow>(logger_, sessionData_, sAdapter_, displayHelper_, commandTarget);
        }
        return nullptr;
    }
} // namespace Core::Commands
