#include "school.hpp"

#include <iostream>
#include <sstream>
#include "../../utilities/inputHandler/inputHandler.hpp"


namespace Core::Types
{
    std::string School::toString() const
    {
        std::stringstream ss("");
        ss << id_ << ". " << name_ << ": " << students_.size() << " students";
        return ss.str();
    }

    std::map<std::string, std::string> School::getAttrs() const
    {
        return {{"name", name_}};
    }

    void School::userConstruct(bool makeFull)
    {
        std::cout << "Creating school from user input\n";
        name_ = Utilities::InputHandler::getAttrAsStringNonEmpty("Name");
        return;
    }

}