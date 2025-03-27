#include <iostream>
#include "inputHandler.hpp"

namespace Utilities
{

    bool InputHandler::isNumber(const std::string& target)
    {
        for(const auto& c : target)
        {
            if(isalpha(c))
            {
                return false;
            }
        }
        return true;
    }

    bool InputHandler::isValidNumber(const std::string& target)
    {
        if(isNumber(target))
        {
            return std::stoi(target) != INT_MAX;
        }
        return false;
    }

    int InputHandler::getNumber()
    {
        int ans = -1;
        std::string buf = "";
        do
        {
            std::cout << "Enter a number: ";
            std::getline(std::cin, buf);
            if(buf.empty())
            {
                return INT_MAX;
            }
        } while (!isNumber(buf));
        return ans = std::stoi(buf);
    }

    int InputHandler::getOption(int optionMin, int optionMax)
    {
        int ans = -1;
        if(optionMin < 0 || optionMax < 0)
        {
            std::cout << "Options have to be a positive number\n";
            return ans;
        }
        
        if(optionMax <= optionMin)
        {
            std::cout << "optionMax has to be greater than optionMin\n";
            return ans;
        }
        bool isValid = false;
        do
        {
            std::cout << "Select option from " << optionMin << " - " << optionMax << "\n";
            ans = getNumber();
            if(ans > optionMax || ans < optionMin)
            {
                    std::cout << "Select a valid option from range " << optionMin << " to " << optionMax << "\n";
            }
            else
            {
                    isValid = true;
            }
        
        } while (!isValid);
        return ans;
    }    

    int InputHandler::getAttrAsNumberNonEmpty(std::string attrName)
    {
        int ans = -1;
        std::string buf = "";
        do
        {
            std::cout << "Enter " << attrName << ": ";
            ans = getNumber();
        } while (ans == INT_MAX);
        return ans;
    }

    std::string InputHandler::getAttrAsString( std::string attrName)
    {
        std::string buf = "";
        std::cout << "Enter " << attrName << ": ";
        std::getline(std::cin, buf);
        return buf;
    }

    std::string InputHandler::getAttrAsStringNonEmpty(std::string attrName)
    {
        std::string buf = "";
        do
        {
            buf = getAttrAsString(attrName);
        } while (buf.empty());
        return buf;
    }

} // namespace Utilities
