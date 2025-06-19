#include "./courseSubjectWeight.hpp"

#include "../../utilities/inputHandler/inputHandler.hpp"

#include <iomanip>
#include <iostream>
#include <sstream>

namespace Core::Types
{

	CourseSubjectWeight::CourseSubjectWeight(
		uint16_t id, double weight, uint16_t courseId, uint16_t subjectId, std::string courseName, std::string subjectName):
		Entry(id, g_tableCourseSubjectWeight),
		weight_(weight),
		courseId_(courseId),
		subjectId_(subjectId),
		courseName_(courseName),
		subjectName_(subjectName)
	{}

	CourseSubjectWeight::CourseSubjectWeight():
		Entry(0, g_tableCourseSubjectWeight),
		weight_(0.0f),
		courseId_(0),
		subjectId_(0),
		courseName_(""),
		subjectName_("")
	{}

	std::shared_ptr<Entry> CourseSubjectWeight::fillGaps(const std::shared_ptr<Entry> other)
	{
		std::shared_ptr<CourseSubjectWeight> concrete = std::static_pointer_cast<CourseSubjectWeight>(other);
		std::shared_ptr<CourseSubjectWeight> refObj   = std::make_shared<CourseSubjectWeight>();
		refObj->id_                                   = id_ == 0 ? concrete->id_ : id_;
		refObj->weight_                               = weight_ == 0.0f ? concrete->weight_ : weight_;
		refObj->courseId_                             = courseId_ == 0 ? concrete->courseId_ : courseId_;
		refObj->subjectId_                            = subjectId_ == 0 ? concrete->subjectId_ : subjectId_;
		return refObj;
	}

	std::string CourseSubjectWeight::toString() const
	{
		std::stringstream ss("");
		ss << "Course " << courseName_ << " (" << courseId_ << ") treats subject " << subjectName_ << " (" << subjectId_ << ") ";
		ss << "with the following weight: " << std::setprecision(2) << weight_;
		return ss.str();
	}

	std::unordered_map<std::string, std::string> CourseSubjectWeight::userConstruct(bool makeFull)
	{
		std::cout << "Constructing weight of a subject to a given course.\n";
		std::unordered_map<std::string, std::string> mappedNewAttrs;
		int tmpHolder = makeFull ? Utilities::InputHandler::getAttrAsNumberNonEmpty("Subject Id")
								 : Utilities::InputHandler::getAttrAsNumber("Subject Id");
		subjectId_    = tmpHolder != INT16_MAX ? tmpHolder : 0;
		if (subjectId_) mappedNewAttrs.insert(std::make_pair("subjectId", std::to_string(subjectId_)));

		tmpHolder = makeFull ? Utilities::InputHandler::getAttrAsNumberNonEmpty("Course Id")
							 : Utilities::InputHandler::getAttrAsNumber("Course Id");
		courseId_ = tmpHolder != INT16_MAX ? tmpHolder : 0;
		if (courseId_) mappedNewAttrs.insert(std::make_pair("courseId", std::to_string(courseId_)));
		// @TODO add float handling. So far that is just a placeholder
		double tmpGrade = makeFull ? Utilities::InputHandler::getAttrAsNumberNonEmpty("Weight")
								   : Utilities::InputHandler::getAttrAsNumber("Weight");
		weight_         = tmpGrade != INT16_MAX ? tmpGrade : 0.0f;
		if (weight_) mappedNewAttrs.insert(std::make_pair("weight", std::to_string(weight_)));
		return mappedNewAttrs;
	}

	std::map<std::string, std::string> CourseSubjectWeight::getAttrs() const
	{
		std::map<std::string, std::string> mappedAttrs;
		mappedAttrs.insert(std::make_pair("weight", std::to_string(weight_)));
		mappedAttrs.insert(std::make_pair("subjectId", std::to_string(subjectId_)));
		mappedAttrs.insert(std::make_pair("courseId", std::to_string(courseId_)));
		return mappedAttrs;
	}

	Entry& CourseSubjectWeight::operator= (const Entry& other)
	{
		if (this == &other) return *this;

		return *this;
	}

}  // namespace Core::Types
