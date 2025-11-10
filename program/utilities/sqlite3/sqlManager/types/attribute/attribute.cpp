#include "attribute.hpp"

#include <sstream>

namespace Utilities::Sql::Types
{

    bool Attribute::isDefaultValid() const
    {
        if (!flags_.contains(AttributeFlag::DEFAULT)) return false;
        if (!defaultValue_.has_value()) return false;

        // Now here is the tricky part - if default value is text BUT attribute type is a real number - that is not a valid
        // default
        std::string defaultHelper = defaultValue_.value();
        if (type_ != AttributeType::SQL_TEXT || type_ != AttributeType::SQL_NULL)  // And blob
        {
            try
            {
                [[maybe_unused]] int tmp = std::stoull(defaultHelper);
                return true;
            }
            catch (const std::exception& e)
            {
                return false;
            }
        }
        return true;
    }

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

    std::string attrTypeToString(const AttributeType& flag)
    {
        switch (flag)
        {
            case AttributeType::SQL_NULL : return "NULL";
            case AttributeType::SQL_INTEGER : return "INTEGER";
            case AttributeType::SQL_REAL : return "REAL";
            case AttributeType::SQL_TEXT : return "TEXT";
            default : return "";
        }
    }

    AttributeType stringToAttrType(const std::string& raw)
    {
        if (raw == "INTEGER") return AttributeType::SQL_INTEGER;
        if (raw == "REAL") return AttributeType::SQL_REAL;
        if (raw == "TEXT") return AttributeType::SQL_TEXT;
        else return AttributeType::SQL_NULL;
    }

}  // namespace Utilities::Sql::Types
