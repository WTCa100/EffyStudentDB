#pragma once

#include <optional>
#include <string>
#include <unordered_set>

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
        REFERENCE,  // Not sure if we will be diving into it
        MISSING
    };

    enum class AttributeType
    {
        SQL_NULL    = 0,
        SQL_INTEGER = 1,
        SQL_REAL    = 2,
        SQL_TEXT    = 3
        // SQL_BLOB = 4
    };

    struct Attribute
    {
        std::string name_;
        AttributeType type_;
        std::unordered_set<AttributeFlag> flags_;
        std::optional<std::string> defaultValue_{ std::nullopt };

        bool isValid() { return !name_.empty(); }

        bool isDefaultValid() const;
    };

    std::string attrFlagToString(const AttributeFlag& flag);
    std::string attrTypeToString(const AttributeType& flag);
    AttributeType stringToAttrType(const std::string& raw);

}  // namespace Utilities::Sql::Types

