#pragma once

#include <string>
#include <map>
#include <vector>

#include "../attribute/attribute.hpp"

// @TODO Implement a way to handle Foreign key and References (like adding ON DELETE SET NULL etc.)

namespace Utilities::Workspace::Sql::Types
{    
    using refKeys = std::vector<std::pair<Attribute, Attribute>>;
    class SqlTable
    {
    private:
        std::string name_;
        std::map<std::string, Attribute> schema_;
        refKeys foreignKeys_;
    public:
        SqlTable(std::string name);
        ~SqlTable() = default;

        std::string getName() const { return name_; }
        void setName(std::string name) { name_ = name; }
        std::map<std::string, Attribute> getSchema() const { return schema_; } 
        refKeys getForeignKeys() const { return foreignKeys_; }
        std::string makeFormula() const;
        // void printTable(std::ostream& stream) const;
        void addToSchema(const Attribute& atr);
        void linkAttributes(Attribute src, Attribute dest);
    };
} // namespace namespace Utilities::Workspace

