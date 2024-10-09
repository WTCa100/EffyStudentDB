#include "table.hpp"

#include <sstream>

namespace Utilities::Workspace::Sql::Types
{

    Table::Table(std::string name) : name_(name), schema_({}), foreignKeys_({}) {}

    void Table::addToSchema(const Attribute& atr)
    {
        // Duplicate or attribute with same name - either way invalid entry
        if(schema_.contains(atr.name_))
        {
            return;
        }
        schema_.insert(std::make_pair(atr.name_, atr));
    }

    Attribute Table::getAttributeByName(const std::string attrName)
    {
        if(schema_.contains(attrName))
        {
            return schema_.at(attrName);
        }
        return Attribute{""};
    }

    std::string Table::makeFormula() const
    {
        std::stringstream ss;

        ss << "CREATE TABLE " << name_ << "(\n";
        uint16_t targetAtrCount = schema_.size() - 1;
        uint16_t currentAtrCount = 0;
        for(const auto& attr : schema_)
        {
            // <attr-name> <attr-type> <attr-flags>
            ss << attr.first << " " << attr.second.type_ << " ";
            for(const auto& flag : attr.second.flags_)
            {
                ss << Utilities::Workspace::Sql::Types::attrFlagToString(flag) << " ";
            }

            if(currentAtrCount < targetAtrCount)
            {
                ss << ",";
            }
            ++currentAtrCount;
            ss << "\n";
        }
        ss << ");\n";
        return ss.str();
    }

    Types::Table defaultSchoolsTable()
    {
        Types::Table schoolTbl("Schools");
        schoolTbl.addToSchema({"id", "INTEGER", {Types::AttributeFlag::PRIMARY_KEY}});
        schoolTbl.addToSchema({"name", "TEXT", {Types::AttributeFlag::NOT_NULL, AttributeFlag::UNIQUE}});
        return schoolTbl;
    }

    Types::Table defaultStudentsTable()
    {
        Types::Table studentTbl("Students");
        studentTbl.addToSchema({"id", "INTEGER", {Types::AttributeFlag::PRIMARY_KEY}});
        studentTbl.addToSchema({"firstName", "TEXT", {Types::AttributeFlag::NOT_NULL}});
        studentTbl.addToSchema({"secondName", "TEXT", {}});
        studentTbl.addToSchema({"lastName", "TEXT", {Types::AttributeFlag::NOT_NULL}});
        studentTbl.addToSchema({"email", "TEXT", {Types::AttributeFlag::NOT_NULL, Types::AttributeFlag::UNIQUE}});
        studentTbl.addToSchema({"schoolId", "INTEGER", {Types::AttributeFlag::NOT_NULL}});
        return studentTbl;
    }

    Types::Table defaultSubjectsTable()
    {
        Types::Table subjectTbl("Subjects");
        subjectTbl.addToSchema({"id", "INTEGER", {Types::AttributeFlag::PRIMARY_KEY}});
        subjectTbl.addToSchema({"name", "TEXT", {Types::AttributeFlag::NOT_NULL, Types::AttributeFlag::UNIQUE}});
        return subjectTbl;
    }

    Types::Table defaultGradesTable()
    {
        Types::Table gradeTbl("Grades");
        gradeTbl.addToSchema({"id", "INTEGER", {Types::AttributeFlag::PRIMARY_KEY}});
        gradeTbl.addToSchema({"studentId", "INTEGER", {Types::AttributeFlag::NOT_NULL}});
        gradeTbl.addToSchema({"subjectId", "INTEGER", {Types::AttributeFlag::NOT_NULL}});
        gradeTbl.addToSchema({"grade", "FLOAT", {Types::AttributeFlag::NOT_NULL}});
        return gradeTbl;
    }    

} // namespace Utilities::Workspace::Sql::Types
