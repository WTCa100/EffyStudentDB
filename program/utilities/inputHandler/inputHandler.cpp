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

        std::string buf = "";
        bool isValid = false;
        
        do
        {
            std::cout << "Select option from " << optionMin << " - " << optionMax << "\n";
            std::getline(std::cin, buf);
            // Validate
            if(!buf.empty())
            {
                if(isNumber(buf))
                {
                ans = std::stoi(buf);
                if(ans > optionMax || ans < optionMin)
                {
                        std::cout << "Select a valid option from range " << optionMin << " to " << optionMax << "\n";
                }
                else
                {
                        isValid = true;
                }
                }
                else
                {
                    std::cout << "Only use numbers.\n";
                }
            }
        } while (!isValid);
        return ans;
    }    
} // namespace Utilities
