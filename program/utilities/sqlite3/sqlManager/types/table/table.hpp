#pragma once

#include "../attribute/attribute.hpp"

#include <string>
#include <unordered_map>
#include <vector>


namespace Utilities::Sql::Types
{
    using refKeys = std::vector<std::string>;

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
        void linkAttributes(const Attribute& src, std::string refferencedTblName, std::string refferencedAttrName);

        Attribute getAttributeByName(std::string name);

        bool isValid() const { return !schema_.empty(); }  // Later it will be checked if at least one primary key is present
    };

    Types::Table defaultSchoolsTable();
    Types::Table defaultStudentsTable();
    Types::Table defaultSubjectsTable();
    Types::Table defaultGradesTable();
    Types::Table defaultCoursesTable();
    Types::Table defaultSubjectToCourseWeightTable();
    Types::Table defaultStudentRequestTable();
    Types::Table defaultCourseAttendeesTable();
}  // namespace Utilities::Sql::Types

