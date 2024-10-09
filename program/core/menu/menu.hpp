#pragma once

#include <memory>

#include "../../utilities/logger/logger.hpp"

class Menu
{
private:
    std::shared_ptr<Utilities::Logger> logger_;
public:
    Menu(std::shared_ptr<Utilities::Logger> logger);
    void showMainMenu();
    // Display main menu
        // I.e. 
        // 1. Add entry
        // 2. Remove entry
        // 3. Alter entry
        // 4. Calculate students
        // 5. Exit (1. are you sure prompt? 2. save prompt?)
    // Display add menu
        // Add school, student, grade, subject, course
    // Display remove menu
        // Remove school, student, grade, subject, course
    // Display alter menu
        // Alter school, student, grade, subject, course
    ~Menu() = default;
};