#pragma once

#include <string>
#include <vector>

namespace Utilities::Sql::Types
{
    enum class AttributeFlag
    {
        PRIMARY_KEY = 0,
        NOT_NULL    = 1,
        UNIQUE      = 2,
        DEFAULT     = 3,
        // Check - not supported
        // Collate - not supported
        REFERENCE // Not sure if we will be diving into it
    };


    struct Attribute
    {
        std::string name_;
        std::string type_;
        std::vector<AttributeFlag> flags_;
        bool isValid() { return !name_.empty(); }
    };

    std::string attrFlagToString(const AttributeFlag& flag);

} // namespace Ut



