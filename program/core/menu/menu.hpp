#pragma once

#include "../../utilities/inputHandler/inputHandler.hpp"
#include "../../utilities/logger/logger.hpp"
#include "../../utilities/sqlite3/sqlAdapter/sqlAdapter.hpp"
#include "../session/action/action.hpp"
#include "../session/data/data.hpp"
#include "../displayHelper/displayHelper.hpp"

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
        Utilities::InputHandler inHandler_;
        const Core::Display::Helper dsplHelper_;

        bool validateAction(const Action& act);
      public:
        Menu(std::shared_ptr<Utilities::Logger> logger, std::shared_ptr<SessionData> sesData, std::shared_ptr<SqlAdapter> sqlAdapter);
        MainMenuOption showMainMenu();
        Action manageDatabase();
        Action createAction();

        void showEntries(const std::string& target) const;
        void showHelp() const;
        void listTables() const;
        bool promptAlterAll(std::string filter, uint16_t count) const;
        void showSelection(const std::vector<std::shared_ptr<Entry>>& selection) const;
        void showEntry(const std::shared_ptr<Entry> entry) const;
        void handleIndirectAction(std::string& command);
        ~Menu() = default;
    };

}  // namespace Core::Display

