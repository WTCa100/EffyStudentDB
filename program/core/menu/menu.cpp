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
        LOG((*logger_), "Displaying Database - getting input");
        std::cout << "What table to display?\n";
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
        switch (op)
        {
        case 1:
        {
            std::string mgmtCmd = manageSchools();
            if(mgmtCmd != "EXIT") returnCommand = "SCHOOL " + mgmtCmd; else returnCommand = "EXIT";
            break;
        }
        case 2:
            sesData_->showGrades();
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

    std::string Menu::manageSchools()
    {
        LOG((*logger_), "Managing schools");
        sesData_->showSchools();
        // @TODO consider adding pages
        // uint16_t pages = schools.size(); 
        // uint16_t currentPage = 1;
        // std::cout << "Displaying Schools";
        std::string commandOption;
        do
        {
            do
            {
                commandOption = getManagementOption();
            } while (!validateCommand(commandOption));
            LOG((*logger_), "Got command: ", commandOption);
        } while (commandOption == "NEXT" || commandOption == "PREV");
        
        return commandOption;
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

    School Menu::constructSchool()
    {
        std::string name;
        std::cout << "Creating a school:\n";
        do
        {
            std::cout << "Name: ";
            std::getline(std::cin, name);
        } while (name.empty());
        return School{0, name, {}};
    }
}