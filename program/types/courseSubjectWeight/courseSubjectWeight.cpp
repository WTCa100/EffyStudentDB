#include<sstream>
#include<iostream>
#include<iomanip>

#include "./courseSubjectWeight.hpp"
#include "../../utilities/inputHandler/inputHandler.hpp"

namespace Core::Types
{
    CourseSubjectWeight::CourseSubjectWeight(uint16_t id, double weight, uint16_t courseId, uint16_t subjectId, std::string courseName, std::string subjectName) :
    Entry(id, g_tableCourseSubjectWeight),
    weight_(weight),
    courseId_(courseId),
    subjectId_(subjectId),
    courseName_(courseName),
    subjectName_(subjectName)
    {
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
        subjectId_ = tmpHolder != INT16_MAX ? tmpHolder : 0;
        if(subjectId_) mappedNewAttrs.insert(std::make_pair("subjectId", std::to_string(subjectId_)));
        
        tmpHolder = makeFull ? Utilities::InputHandler::getAttrAsNumberNonEmpty("Course Id") 
                             : Utilities::InputHandler::getAttrAsNumber("Course Id");
        courseId_ = tmpHolder != INT16_MAX ? tmpHolder : 0;
        if(courseId_) mappedNewAttrs.insert(std::make_pair("courseId", std::to_string(courseId_)));
        // @TODO add float handling. So far that is just a placeholder
        double tmpGrade = makeFull ? Utilities::InputHandler::getAttrAsNumberNonEmpty("Weight")
                                   : Utilities::InputHandler::getAttrAsNumber("Weight");
        weight_ = tmpGrade != INT16_MAX ? tmpGrade : 0.0f;
        if(weight_) mappedNewAttrs.insert(std::make_pair("weight",std::to_string(weight_)));
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

    Entry& CourseSubjectWeight::operator=(const Entry& other)
    {
        if(this == &other) return *this;

        return *this;
    }

} // namespace Core::Types
