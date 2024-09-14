#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "../attribute/attribute.hpp"

// @TODO Implement a way to handle Foreign key and References (like adding ON DELETE SET NULL etc.)

namespace Utilities::Workspace::Sql::Types
{   
    using refKeys = std::vector<std::pair<Attribute, Attribute>>;
    class Table
    {
    private:
        std::string name_;
        std::unordered_map<std::string, Attribute> schema_;
        refKeys foreignKeys_;
    public:
        Table(std::string name);
        ~Table() = default;

        std::string getName() const { return name_; }
        void setName(std::string name) { name_ = name; }
        std::unordered_map<std::string, Attribute> getSchema() const { return schema_; } 
        refKeys getForeignKeys() const { return foreignKeys_; }
        std::string makeFormula() const;
        // void printTable(std::ostream& stream) const;
        void addToSchema(const Attribute& atr);
        void linkAttributes(Attribute src, std::string refferencedTblName, std::string refferencedAttrName);
        
        Attribute getAttributeByName(std::string name); 
        
        bool isValid() const { return !schema_.empty(); } // Later it will be checked if at least one primary key is present
        // void linkAttributes(Attribute src, Attribute dest);
    };

     
    Types::Table defaultSchoolsTable();
    Types::Table defaultStudentsTable();
    Types::Table defaultSubjectsTable();
    Types::Table defaultGradesTable();
} //namespace Utilities::Workspace::Sql::Types

