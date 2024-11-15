#pragma once

#include <memory>

namespace Utilities
{
    class InputHandler
    {
    private:
    public:
        // Validators
        static bool isNumber(const std::string& target);

        InputHandler()  = default;
        ~InputHandler() = default;
        int getOption(int optionMin, int optionMax);
    };
}