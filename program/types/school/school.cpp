#include "school.hpp"

#include "../../utilities/inputHandler/inputHandler.hpp"

#include <iostream>
#include <sstream>

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
        return {
            { "name", name_ }
        };
    }

    std::unordered_map<std::string, std::string> School::userConstruct(bool makeFull)
    {
        std::unordered_map<std::string, std::string> mappedNewAttrs;
        std::cout << "Creating school from user input\n";
        name_ = makeFull ? Utilities::InputHandler::getAttrAsStringNonEmpty("Name")
                         : Utilities::InputHandler::getAttrAsString("Name");
        if (!name_.empty()) mappedNewAttrs.insert(std::make_pair("name", name_));
        return mappedNewAttrs;
    }

    std::shared_ptr<Entry> School::fillGaps(const std::shared_ptr<Entry> other)
    {
        std::shared_ptr<School> concrete = std::static_pointer_cast<School>(other);
        std::shared_ptr<School> refObj   = std::make_shared<School>();
        refObj->id_                      = id_ == 0 ? concrete->id_ : id_;
        refObj->name_                    = name_.empty() ? concrete->name_ : name_;
        return refObj;
    }

    Entry& School::operator= (const Entry& other)
    {
        if (this == &other) return *this;
        // Cast to aproporiate type
        const School* otherSchool = dynamic_cast<const School*>(&other);
        id_                       = otherSchool->id_;
        name_                     = otherSchool->name_;
        students_                 = otherSchool->students_;
        return *this;
    }
}  // namespace Core::Types