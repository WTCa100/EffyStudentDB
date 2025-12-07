#pragma once

#include "../../utilities/inputHandler/inputHandler.hpp"
#include "../../utilities/logger/logger.hpp"


#include <memory>

namespace Core::Display
{

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
        Utilities::InputHandler inHandler_;

      public:
        Menu(std::shared_ptr<Utilities::Logger> logger);
        MainMenuOption showMainMenu();

        ~Menu() = default;
    };

}  // namespace Core::Display

