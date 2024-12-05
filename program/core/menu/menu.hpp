#pragma once

#include <memory>

#include "../session/data/data.hpp"
#include "../../utilities/logger/logger.hpp"
#include "../../utilities/inputHandler/inputHandler.hpp"

namespace Core::Display
{

    enum MainMenuOption
    {
        manageDb  = 1,
        handleRqs    ,
        exit
    };

    class Menu
    {
    private:
        std::shared_ptr<Utilities::Logger> logger_;
        std::shared_ptr<SessionData> sesData_;
        std::unique_ptr<Utilities::InputHandler> inHandler_;
        // Consider adding session data shared ptr here to easily handler
        // content display
    public:
        Menu(std::shared_ptr<Utilities::Logger> logger, std::shared_ptr<SessionData> sesData);
        MainMenuOption showMainMenu();
        void manageDatabase();
        // Change in plan was made and now the display will a little bit differently to make it more readable for the client.
        // Display main menu
            // I.e. 
            // 1. Show database
            // 2. Handle requests
            // 3. Exit (1. are you sure prompt? 2. save prompt?)
        // The show database is a interactive menu that allows the user to search it (manually or automatically), add, remove and alter entries
        // This would work as follow:
        // User enters 1. -> Display changes to the show db. Then options are available by typping a command: "ADD/ALTER/REMOVE/FIND <type>"
        // Aproporiate type builder will be handled after entering a command.
        // ADD, REMOVE and ALTER will work on the local memory. Find will call a SQL query to obtain the id with given params.
        ~Menu() = default;
    };

} // namespace core::display

