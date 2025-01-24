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
        std::cout << "Which table you want to manage?\n";
        std::cout << "1. Schools\n";
        std::cout << "2. Students with grades\n";
        std::cout << "3. Subjects\n";
        std::cout << "4. Courses\n";
        // Should we keep the weights in separate tables?
        std::cout << "5. Student Request\n";
        std::cout << "6. Exit\n";
        int op = inHandler_->getOption(1, 6);
        LOG((*logger_), "Got option ", op);

        // For now - this will get overhauled later
        std::string returnCommand;
        std::string mgmtCmd;
        switch (op)
        {
        case 1:
            mgmtCmd = manageSchools();
            if(mgmtCmd != "EXIT") returnCommand = "SCHOOL " + mgmtCmd; else returnCommand = "EXIT";
            break;

        case 2:
            mgmtCmd = manageStudents();
            if(mgmtCmd != "EXIT") returnCommand = "STUDENT " + mgmtCmd; else returnCommand = "EXIT";
            break;
        case 3:
            sesData_->showSubjects();
            break;
        case 4:
            sesData_->showCourses();
            break;
        case 5:
            sesData_->showStudentRequests();
            break;
        case 6:
            LOG((*logger_), "Going back to main menu");
        default:
            returnCommand = "EXIT";
            break;
        }
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

    std::string Menu::manageSchools()
    {
        LOG((*logger_), "Managing schools");
        // @TODO consider adding pages
        // uint16_t pages = schools.size(); 
        // uint16_t currentPage = 1;
        // std::cout << "Displaying Schools";
        
        std::string command;
        do
        {
            sesData_->showSchools();
            command = makeCommand();
        } while (command == "NEXT" || command == "PREV");
        return command;
    }

    std::string Menu::manageStudents()
    {
        LOG((*logger_), "Manage Students");
        std::string command;
        do
        {
            sesData_->showStudents();
            command = makeCommand();
            std::cout << "Command: " << command << "\n";
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
                if(!(tokenizedCmd.size() > 1))
                {
                    return false;
                }

                std::string cmdIdPart = tokenizedCmd.at(1);
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