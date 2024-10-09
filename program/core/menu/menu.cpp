#include "menu.hpp"

Menu::Menu(std::shared_ptr<Utilities::Logger> logger)
{
    if(!logger)
    {
        std::cout << "Logger is null! Can't save logs\n";
    }
    else
    {
        logger_ = logger;
        LOG((*logger_), "Menu object created");
    }
}

void Menu::showMainMenu()
{
    LOG((*logger_), "Showing main menu");
    std::cout << "-=# Main Menu #=-\n";
    std::cout << "1. Add entry (like school, subject, etc.).\n";
    std::cout << "2. Remove entry.\n";
    std::cout << "3. Alter entry.\n";
    std::cout << "4. Calculate.\n";
    std::cout << "5. Exit\n";
}