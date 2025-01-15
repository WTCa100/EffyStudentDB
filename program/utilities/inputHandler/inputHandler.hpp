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

        static int getNumber();
        int getOption(int optionMin, int optionMax);
        static int getAttrAsNumber(std::string attrName = "Attribute");
        static std::string getAttrAsString(std::string attrName = "Attribute");
        static std::string getAttrAsStringNonEmpty(std::string attrName = "Attribute");
    };
}