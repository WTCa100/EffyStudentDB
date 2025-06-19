#include "attribute.hpp"

#include <sstream>

namespace Utilities::Sql::Types
{
    std::string attrFlagToString(const AttributeFlag& flag)
    {
        switch (flag)
        {
            case AttributeFlag::DEFAULT : return "DEFAULT";
            case AttributeFlag::NOT_NULL : return "NOT NULL";
            case AttributeFlag::PRIMARY_KEY : return "PRIMARY KEY";
            case AttributeFlag::REFERENCE : return "REFFERENCE";
            case AttributeFlag::UNIQUE : return "UNIQUE";
            case AttributeFlag::MISSING : return "MISSING";
            default : return "";
        }
    }

}  // namespace Utilities::Sql::Types
