#include "table.hpp"
#include <iostream>

#include "../../../../common/constants.hpp"

#include <sstream>

using namespace Utilities::Common::Constants;

namespace Utilities::Sql::Types
{

    Table::Table(std::string name):
        name_(name),
        schema_({}),
        foreignKeys_({})
    {}

    void Table::addToSchema(const Attribute& atr)
    {
        // Duplicate - invalid entry
        if (schema_.contains(atr.name_)) { return; }
        schema_.insert(std::make_pair(atr.name_, atr));
    }

    Attribute Table::getAttributeByName(const std::string attrName)
    {
        if (schema_.contains(attrName)) { return schema_.at(attrName); }
        return Attribute{ "", Types::AttributeType::SQL_NULL, {Types::AttributeFlag::MISSING}, {} };
    }

    std::string Table::makeFormula() const
    {
        std::stringstream ss;
        std::cout << "DBG: Making formula for " << name_ << "\n";

        ss << "CREATE TABLE " << name_ << "(\n";
        uint16_t targetAtrCount  = schema_.size() - 1;
        uint16_t currentAtrCount = 0;

        // Attribute declarations
        for (const auto& attr : schema_)
        {
            // <attr-name> <attr-type> <attr-flags> <attr-default>
            const Attribute& currentAttr = attr.second;  
            ss << currentAttr.name_ << " " << attrTypeToString(currentAttr.type_) << " ";
            for (const auto& flag : currentAttr.flags_) 
            {
                if (flag == AttributeFlag::DEFAULT)
                {
                    // Default flag must always be the last in this case
                    continue;
                } 
                ss << attrFlagToString(flag) << " ";
            }

            // Check Default flag validity
            if(currentAttr.isDefaultValid())
            {
                ss << attrFlagToString(AttributeFlag::DEFAULT)  << " " << currentAttr.defaultValue_.value() << " ";
            }
            
            if (currentAtrCount < targetAtrCount || !foreignKeys_.empty()) { ss << ","; }
            ++currentAtrCount;
            ss << "\n";
        }

        for (const auto& link : foreignKeys_) { ss << link << "\n"; }

        ss << ");\n";
        return ss.str();
    }

    void Table::linkAttributes(const Attribute& src, std::string refferencedTblName, std::string refferencedAttrName)
    {
        std::stringstream ss;
        ss << "FOREIGN KEY (" << src.name_ << ") REFERENCES " << refferencedTblName << "(" << refferencedAttrName << ")";
        foreignKeys_.push_back(ss.str());
    }

    Types::Table defaultSchoolsTable()
    {
        Types::Table schoolTbl(g_tableSchools);
        schoolTbl.addToSchema({ "id", Types::AttributeType::SQL_INTEGER, { Types::AttributeFlag::PRIMARY_KEY } });
        schoolTbl.addToSchema({
            "name", Types::AttributeType::SQL_TEXT, { Types::AttributeFlag::NOT_NULL, AttributeFlag::UNIQUE }
        });
        return schoolTbl;
    }

    Types::Table defaultStudentsTable()
    {
        Types::Table studentTbl(g_tableStudents);
        studentTbl.addToSchema({ "id", Types::AttributeType::SQL_INTEGER, { Types::AttributeFlag::PRIMARY_KEY } });
        studentTbl.addToSchema({ "firstName", Types::AttributeType::SQL_TEXT, { Types::AttributeFlag::NOT_NULL } });
        studentTbl.addToSchema({ "secondName", Types::AttributeType::SQL_TEXT, {} });
        studentTbl.addToSchema({ "lastName", Types::AttributeType::SQL_TEXT, { Types::AttributeFlag::NOT_NULL } });
        studentTbl.addToSchema({
            "email", Types::AttributeType::SQL_TEXT, { Types::AttributeFlag::NOT_NULL, Types::AttributeFlag::UNIQUE }
        });
        studentTbl.addToSchema({ "schoolId", Types::AttributeType::SQL_INTEGER, { Types::AttributeFlag::NOT_NULL } });
        studentTbl.linkAttributes(studentTbl.getAttributeByName("schoolId"), "Schools", "id");
        return studentTbl;
    }

    Types::Table defaultSubjectsTable()
    {
        Types::Table subjectTbl(g_tableSubjects);
        subjectTbl.addToSchema({ "id", Types::AttributeType::SQL_INTEGER, { Types::AttributeFlag::PRIMARY_KEY } });
        subjectTbl.addToSchema({
            "name", Types::AttributeType::SQL_TEXT, { Types::AttributeFlag::NOT_NULL, Types::AttributeFlag::UNIQUE }
        });
        return subjectTbl;
    }

    Types::Table defaultGradesTable()
    {
        Types::Table gradeTbl(g_tableGrades);
        gradeTbl.addToSchema({ "id", Types::AttributeType::SQL_INTEGER, { Types::AttributeFlag::PRIMARY_KEY } });
        gradeTbl.addToSchema({ "studentId", Types::AttributeType::SQL_INTEGER, { Types::AttributeFlag::NOT_NULL } });
        gradeTbl.addToSchema({ "subjectId", Types::AttributeType::SQL_INTEGER, { Types::AttributeFlag::NOT_NULL } });
        // A float is always a REAL. Ref. https://www.sqlite.org/datatype3.html 3.1.1.
        gradeTbl.addToSchema({ "grade", Types::AttributeType::SQL_REAL, { Types::AttributeFlag::NOT_NULL } });
        gradeTbl.linkAttributes(gradeTbl.getAttributeByName("studentId"), "Students", "id");
        gradeTbl.linkAttributes(gradeTbl.getAttributeByName("subjectId"), "Subjects", "id");
        return gradeTbl;
    }

    Types::Table defaultCoursesTable()
    {
        Types::Table courseTbl(g_tableCourses);
        courseTbl.addToSchema({ "id", Types::AttributeType::SQL_INTEGER, { Types::AttributeFlag::PRIMARY_KEY } });
        courseTbl.addToSchema({
            "name", Types::AttributeType::SQL_TEXT, { Types::AttributeFlag::NOT_NULL, Types::AttributeFlag::UNIQUE }
        });
        courseTbl.addToSchema({ "isOpen", Types::AttributeType::SQL_INTEGER,
            { Types::AttributeFlag::NOT_NULL } });  // 0 - for internal usage only; 1 - Accepting; 2 - not accepting
            courseTbl.addToSchema({ "minStudents", Types::AttributeType::SQL_INTEGER, { Types::AttributeFlag::NOT_NULL } });
            courseTbl.addToSchema({ "maxStudents", Types::AttributeType::SQL_INTEGER, { Types::AttributeFlag::NOT_NULL } });
            courseTbl.addToSchema({ "baseMinimalPoints", Types::AttributeType::SQL_INTEGER, { Types::AttributeFlag::NOT_NULL } });
            courseTbl.addToSchema({ "recrutingTurn", Types::AttributeType::SQL_INTEGER, { Types::AttributeFlag::NOT_NULL } });
        return courseTbl;
    }

    Types::Table defaultSubjectToCourseWeightTable()
    {
        Types::Table subjectWeightMap(g_tableCourseSubjectWeight);
        subjectWeightMap.addToSchema({ "id", Types::AttributeType::SQL_INTEGER, { Types::AttributeFlag::PRIMARY_KEY } });
        subjectWeightMap.addToSchema({ "courseId", Types::AttributeType::SQL_INTEGER, { Types::AttributeFlag::NOT_NULL } });
        subjectWeightMap.addToSchema({ "subjectId", Types::AttributeType::SQL_INTEGER, { Types::AttributeFlag::NOT_NULL } });
        subjectWeightMap.addToSchema({ "weight", Types::AttributeType::SQL_REAL, { Types::AttributeFlag::NOT_NULL } });
        subjectWeightMap.linkAttributes(subjectWeightMap.getAttributeByName("subjectId"), "Subjects", "id");
        subjectWeightMap.linkAttributes(subjectWeightMap.getAttributeByName("courseId"), "Courses", "id");
        return subjectWeightMap;
    }

    Types::Table defaultStudentRequestTable()
    {
        Types::Table studentReq(g_tableStudentRequest);
        studentReq.addToSchema({ "id", Types::AttributeType::SQL_INTEGER, { Types::AttributeFlag::PRIMARY_KEY } });
        studentReq.addToSchema({ "studentId", Types::AttributeType::SQL_INTEGER, { Types::AttributeFlag::NOT_NULL } });
        studentReq.addToSchema({ "courseId", Types::AttributeType::SQL_INTEGER, { Types::AttributeFlag::NOT_NULL } });
        studentReq.addToSchema({ "requestStatus", Types::AttributeType::SQL_INTEGER, { Types::AttributeFlag::NOT_NULL } });
        studentReq.linkAttributes(studentReq.getAttributeByName("studentId"), "Students", "id");
        studentReq.linkAttributes(studentReq.getAttributeByName("courseId"), "Courses", "id");
        return studentReq;
    }

    Types::Table defaultCourseAttendeesTable()
    {
        Types::Table attendees(g_tableCourseAttendees);
        attendees.addToSchema({ "id", Types::AttributeType::SQL_INTEGER, { Types::AttributeFlag::PRIMARY_KEY } });
        attendees.addToSchema({ "studentId", Types::AttributeType::SQL_INTEGER, { Types::AttributeFlag::NOT_NULL } });
        attendees.addToSchema({ "courseId", Types::AttributeType::SQL_INTEGER, { Types::AttributeFlag::NOT_NULL } });
        attendees.addToSchema({ "points", Types::AttributeType::SQL_REAL, {Types::AttributeFlag::NOT_NULL, Types::AttributeFlag::DEFAULT}, "100"});
        return attendees;
    }

}  // namespace Utilities::Sql::Types
