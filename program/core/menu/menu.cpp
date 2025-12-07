#include "menu.hpp"

#include "../../utilities/inputHandler/inputHandler.hpp"

namespace Core::Display
{
    Menu::Menu(
        std::shared_ptr<Utilities::Logger> logger):
        logger_(logger)
    {
        LOG((*logger_), "Menu object created");
    }

    MainMenuOption Menu::showMainMenu()
    {
        MainMenuOption op;
        LOG((*logger_), "Showing main menu");
        std::cout << "-=# Main Menu #=-\n";
        std::cout << "1. Manage Database\n";
        std::cout << "2. Handle student requests\n";
        std::cout << "3. Exit.\n";
        op = static_cast<MainMenuOption>(inHandler_.getOption(1, 3));
        LOG((*logger_), "Got option ", op);
        return op;
    }
}  // namespace Core::Display