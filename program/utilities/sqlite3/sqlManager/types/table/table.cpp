#include "table.hpp"

#include <sstream>

namespace Utilities::Workspace::Sql::Types
{
    SqlTable::SqlTable(std::string name) : name_(name), schema_({}), foreignKeys_({}) {}

    void SqlTable::addToSchema(const Attribute& atr)
    {
        // Duplicate or attribute with same name - either way invalid entry
        if(schema_.contains(atr.name_))
        {
            return;
        }
        schema_.insert(std::make_pair(atr.name_, atr));
    }

    std::string SqlTable::makeFormula() const
    {
        std::stringstream ss;

        ss << "CREATE TABLE " << name_ << "(\n";
        for(const auto& attr : schema_)
        {
            // <attr-name> <attr-type> <attr-flags>
            ss << attr.first << " " << attr.second.type_ << " ";
            for(const auto& flag : attr.second.flags_)
            {
                ss << Utilities::Workspace::Sql::Types::attrFlagToString(flag) << " ";
            }
            ss << "\n";
        }
        ss << ");\n";
        return ss.str();
    }

} // namespace Utilities::Workspace::Sql::Types
