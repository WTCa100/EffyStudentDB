#pragma once

#include "../../utilities/inputHandler/inputHandler.hpp"
#include "../../utilities/logger/logger.hpp"
#include "../session/action/action.hpp"
#include "../session/data/data.hpp"

#include <memory>
#include <set>

namespace Core::Display
{

    const std::string E_NoSuchTable("404_NO_TABLE");

    enum MainMenuOption
    {
        manageDb  = 1,
        handleRqs = 2,
        exit      = 3
    };

    class Menu
    {
      private:
        std::shared_ptr<Utilities::Logger> logger_;
        std::shared_ptr<SessionData> sesData_;
        std::unique_ptr<Utilities::InputHandler> inHandler_;

        bool validateAction(const Action& act);
        // Consider adding session data shared ptr here to easily handler
        // content display
      public:
        Menu(std::shared_ptr<Utilities::Logger> logger, std::shared_ptr<SessionData> sesData);
        MainMenuOption showMainMenu();
        std::string getManagementOption() const;
        // Manage each table separately

        Action manageDatabase();
        Action createAction();

        void showEntries(const std::string& target) const;
        void showHelp() const;
        void listTables() const;
        bool promptDeleteAll(std::string filter, uint16_t count) const;
        // Change in plan was made and now the display will a little bit differently to make it more readable for the client.
        // Display main menu
        // I.e.
        // 1. Show database
        // 2. Handle requests
        // 3. Exit (1. are you sure prompt? 2. save prompt?)
        // The show database is a interactive menu that allows the user to search it (manually or automatically), add, remove and
        // alter entries This would work as follow: User enters 1. -> Display changes to the show db. Then options are available
        // by typping a command: "ADD/ALTER/REMOVE/FIND <type>" Aproporiate type builder will be handled after entering a command.
        // ADD, REMOVE and ALTER will work on the local memory. Find will call a SQL query to obtain the id with given params.
        ~Menu() = default;
    };

}  // namespace Core::Display

