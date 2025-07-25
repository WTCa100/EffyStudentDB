#pragma once

#include "../common/constants.hpp"

#include <memory>
#include <optional>

namespace Utilities
{

    class InputHandler
    {
      private:
      public:
        // Validators
        static bool isNumber(const std::string& target);
        static bool isValidNumber(const std::string& target);

        InputHandler()  = default;
        ~InputHandler() = default;

        static int getNumber();
        int getOption(int optionMin, int optionMax);
        static int getAttrAsNumberNonEmpty(std::string attrName = "Attribute");
        static int getAttrAsNumber(std::string attrName = "Attribute");
        static std::string getAttrAsString(std::string attrName = "Attribute");
        static std::string getAttrAsStringNonEmpty(std::string attrName = "Attribute");
        static std::string getStringBeauty(std::optional<std::string> prompt = std::nullopt);
        static std::string getStringUgly(std::optional<std::string> prompt = std::nullopt);
        static char getYesOrNo();

        static void beautify(std::string& target);
        static std::string toLower(const std::string& target);
        static std::string toUpper(const std::string& target);
    };
}  // namespace Utilities