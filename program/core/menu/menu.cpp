#include "menu.hpp"

namespace Core::Display
{
    Menu::Menu(std::shared_ptr<Utilities::Logger> logger, std::shared_ptr<SessionData> sesData)
    {
        logger_ = logger;
        sesData_ = sesData;
        LOG((*logger_), "Menu object created");
        inHandler_ = std::make_unique<Utilities::InputHandler>();
    }

    void Menu::displayDatabase()
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
        switch (op)
        {
        case 1:
            sesData_->showSchools();
            break;
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
            break;
        }
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