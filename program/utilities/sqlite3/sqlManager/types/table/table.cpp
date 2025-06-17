#include "table.hpp"

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
		// Duplicate or attribute with same name - either way invalid entry
		if (schema_.contains(atr.name_)) { return; }
		schema_.insert(std::make_pair(atr.name_, atr));
	}

	Attribute Table::getAttributeByName(const std::string attrName)
	{
		if (schema_.contains(attrName)) { return schema_.at(attrName); }
		return Attribute{ "" };
	}

	std::string Table::makeFormula() const
	{
		std::stringstream ss;

		ss << "CREATE TABLE " << name_ << "(\n";
		uint16_t targetAtrCount  = schema_.size() - 1;
		uint16_t currentAtrCount = 0;

		// Attribute declarations
		for (const auto& attr : schema_)
		{
			// <attr-name> <attr-type> <attr-flags>
			ss << attr.first << " " << attr.second.type_ << " ";
			for (const auto& flag : attr.second.flags_) { ss << Utilities::Sql::Types::attrFlagToString(flag) << " "; }

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

	// TODO: Avoid using magic values - change later to global variables

	Types::Table defaultSchoolsTable()
	{
		Types::Table schoolTbl(g_tableSchools);
		schoolTbl.addToSchema({ "id", "INTEGER", { Types::AttributeFlag::PRIMARY_KEY } });
		schoolTbl.addToSchema({
			"name", "TEXT", { Types::AttributeFlag::NOT_NULL, AttributeFlag::UNIQUE }
        });
		return schoolTbl;
	}

	Types::Table defaultStudentsTable()
	{
		Types::Table studentTbl(g_tableStudents);
		studentTbl.addToSchema({ "id", "INTEGER", { Types::AttributeFlag::PRIMARY_KEY } });
		studentTbl.addToSchema({ "firstName", "TEXT", { Types::AttributeFlag::NOT_NULL } });
		studentTbl.addToSchema({ "secondName", "TEXT", {} });
		studentTbl.addToSchema({ "lastName", "TEXT", { Types::AttributeFlag::NOT_NULL } });
		studentTbl.addToSchema({
			"email", "TEXT", { Types::AttributeFlag::NOT_NULL, Types::AttributeFlag::UNIQUE }
        });
		studentTbl.addToSchema({ "schoolId", "INTEGER", { Types::AttributeFlag::NOT_NULL } });
		studentTbl.linkAttributes(studentTbl.getAttributeByName("schoolId"), "Schools", "id");
		return studentTbl;
	}

	Types::Table defaultSubjectsTable()
	{
		Types::Table subjectTbl(g_tableSubjects);
		subjectTbl.addToSchema({ "id", "INTEGER", { Types::AttributeFlag::PRIMARY_KEY } });
		subjectTbl.addToSchema({
			"name", "TEXT", { Types::AttributeFlag::NOT_NULL, Types::AttributeFlag::UNIQUE }
        });
		return subjectTbl;
	}

	Types::Table defaultGradesTable()
	{
		Types::Table gradeTbl(g_tableGrades);
		gradeTbl.addToSchema({ "id", "INTEGER", { Types::AttributeFlag::PRIMARY_KEY } });
		gradeTbl.addToSchema({ "studentId", "INTEGER", { Types::AttributeFlag::NOT_NULL } });
		gradeTbl.addToSchema({ "subjectId", "INTEGER", { Types::AttributeFlag::NOT_NULL } });
		gradeTbl.addToSchema({ "grade", "FLOAT", { Types::AttributeFlag::NOT_NULL } });
		gradeTbl.linkAttributes(gradeTbl.getAttributeByName("studentId"), "Students", "id");
		gradeTbl.linkAttributes(gradeTbl.getAttributeByName("subjectId"), "Subjects", "id");
		return gradeTbl;
	}

	Types::Table defaultCoursesTable()
	{
		Types::Table courseTbl(g_tableCourses);
		courseTbl.addToSchema({ "id", "INTEGER", { Types::AttributeFlag::PRIMARY_KEY } });
		courseTbl.addToSchema({
			"name", "TEXT", { Types::AttributeFlag::NOT_NULL, Types::AttributeFlag::UNIQUE }
        });
		courseTbl.addToSchema({ "minStudents", "INTEGER", { Types::AttributeFlag::NOT_NULL } });
		courseTbl.addToSchema({ "maxStudents", "INTEGER", { Types::AttributeFlag::NOT_NULL } });
		courseTbl.addToSchema({ "baseMinimalPoints", "INTEGER", { Types::AttributeFlag::NOT_NULL } });
		return courseTbl;
	}

	Types::Table defaultSubjectToCourseWeightTable()
	{
		Types::Table subjectWeightMap(g_tableCourseSubjectWeight);
		subjectWeightMap.addToSchema({ "id", "INTEGER", { Types::AttributeFlag::PRIMARY_KEY } });
		subjectWeightMap.addToSchema({ "courseId", "INTEGER", { Types::AttributeFlag::NOT_NULL } });
		subjectWeightMap.addToSchema({ "subjectId", "INTEGER", { Types::AttributeFlag::NOT_NULL } });
		subjectWeightMap.addToSchema({ "weight", "REAL", { Types::AttributeFlag::NOT_NULL } });
		subjectWeightMap.linkAttributes(subjectWeightMap.getAttributeByName("subjectId"), "Subjects", "id");
		subjectWeightMap.linkAttributes(subjectWeightMap.getAttributeByName("courseId"), "Courses", "id");
		return subjectWeightMap;
	}

	Types::Table defaultStudentRequestTable()
	{
		Types::Table studentReq(g_tableStudentRequest);
		studentReq.addToSchema({ "id", "INTEGER", { Types::AttributeFlag::PRIMARY_KEY } });
		studentReq.addToSchema({ "studentId", "INTEGER", { Types::AttributeFlag::NOT_NULL } });
		studentReq.addToSchema({ "courseId", "INTEGER", { Types::AttributeFlag::NOT_NULL } });
		studentReq.addToSchema({ "requestStatus", "INTEGER", { Types::AttributeFlag::NOT_NULL } });
		studentReq.linkAttributes(studentReq.getAttributeByName("studentId"), "Students", "id");
		studentReq.linkAttributes(studentReq.getAttributeByName("courseId"), "Courses", "id");
		return studentReq;
	}

}  // namespace Utilities::Sql::Types
