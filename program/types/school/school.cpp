#include "school.hpp"

#include <sstream>

namespace Core::Types
{
    std::string School::toString() const
    {
        std::stringstream ss("");
        ss << id_ << ". " << name_ << ": " << students_.size() << " students";
        return ss.str();
    }
}