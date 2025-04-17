#include "menu.hpp"

#include "../../utilities/common/stringManip.hpp"

namespace Core::Display
{
    Menu::Menu(std::shared_ptr<Utilities::Logger> logger, std::shared_ptr<SessionData> sesData)
    {
        logger_ = logger;
        sesData_ = sesData;
        LOG((*logger_), "Menu object created");
        inHandler_ = std::make_unique<Utilities::InputHandler>();
    }

    std::string Menu::manageDatabase()
    {
        LOG((*logger_), "Choosing table to manage - getting input");
        std::cout << "Select table to manage\n"; 
        
        std::cout << "\"Schools\"\n";
        std::cout << "\"Students\" (With grades)\n";
        std::cout << "\"Subjects\"\n";
        std::cout << "\"Courses\"\n";
        std::cout << "\"StudentRequest\"\n";
        std::cout << "\"CourseSubjectWeight\"\n";
        std::cout << "Exit\n";
        std::string target = inHandler_->getStringBeauty("Table Name");
        
        if(inHandler_->toUpper(target) == "EXIT")
        {
            return "EXIT";
        }
        std::string returnCommand = manageEntries(target);
        
        // If no table with such name present - return EXIT
        if( returnCommand == E_NoSuchTable) returnCommand = "EXIT";
        return returnCommand;
    }


    std::string Menu::makeCommand()
    {
        std::string commandOption;
        do
        {
            commandOption = getManagementOption();
        } while (!validateCommand(commandOption));
        LOG((*logger_), "Got command: ", commandOption);
        return commandOption;  
    }

    std::string Menu::manageEntries(const std::string& target)
    {
        LOG((*logger_), "Managing entry table: ", target);
        const std::unique_ptr<concreteTypeList> concreteList = sesData_->getEntries(target);

        if(!concreteList) 
        {
            LOG((*logger_), "Could not find table: ", target);
            return E_NoSuchTable;
        }

        std::string command;
        do
        {
            std::cout << "Displaying " << target << ": \n";
            std::cout << concreteList->size() << " entries\n";
            for(const auto& entry : *concreteList)
            {
                std::cout << entry.second.get()->toString() << "\n";
            }
            command = makeCommand();
        } while (command == "NEXT" || command == "PREV");
        return command;

    }

    bool Menu::validateCommand(std::string cmd)
    {
        if(cmd.empty())
        {
            return false;
        }

        std::vector<std::string> tokenizedCmd = Utilities::Common::tokenize(cmd, ' ');

        std::string cmdCore = tokenizedCmd.at(0);

        if(cmdCore == "NEXT" ||
           cmdCore == "PREV" ||
           cmdCore == "ADD"  ||
           cmdCore == "ALTER" ||
           cmdCore == "REMOVE" ||
           cmdCore == "FIND" ||
           cmdCore == "EXIT")
        {
            if(cmdCore == "ALTER" || cmdCore == "REMOVE")
            {
                std::string cmdIdPart = "";
                if((tokenizedCmd.size() > 1))
                {
                    cmdIdPart = tokenizedCmd.at(1);
                }
                
                return Utilities::InputHandler::isNumber(cmdIdPart);
            }
            return true;
        }
        return false;
    }

    std::string Menu::getManagementOption() const
    {
            LOG((*logger_), "Getting management option");
            std::cout << "Options:\n";
            std::cout << "NEXT - move 1 page fruther\n";
            std::cout << "PREV - move 1 page back\n";
            std::cout << "ADD - proceed to create a new entry\n";
            std::cout << "ALTER <targetID> - proceed to update entry with provided targetId with new values\n";
            std::cout << "REMOVE <targetID> - proceed to delete a given entry\n";
            std::cout << "FIND - proceed to get entries that match at least one parameter\n";
            std::cout << "EXIT - exits this view\n";
            std::string buf;
            std::getline(std::cin, buf);
            return buf;
    }

    MainMenuOption Menu::showMainMenu()
    {
        MainMenuOption op;
        LOG((*logger_), "Showing main menu");
        std::cout << "-=# Main Menu #=-\n";
        std::cout << "1. Manage Database\n";
        std::cout << "2. Handle student requests\n";
        std::cout << "3. Exit.\n";
        op = static_cast<MainMenuOption>(inHandler_->getOption(1, 3));
        LOG((*logger_), "Got option ", op);
        return op;
    }
}