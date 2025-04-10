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

    std::unordered_map<std::string, std::string> School::userConstruct(bool makeFull)
    {
        std::cout << "Creating school from user input\n";
        name_ = makeFull ? Utilities::InputHandler::getAttrAsStringNonEmpty("Name") : Utilities::InputHandler::getAttrAsString("Name");
        return {};
    }

    Entry& School::operator=(const Entry& other)
    {
        if(this == &other) return *this;
        // Cast to aproporiate type
        const School* otherSchool = dynamic_cast<const School*>(&other);
        id_              = otherSchool->id_;
        name_            = otherSchool->name_;
        students_        = otherSchool->students_;
        return *this;
    }
}