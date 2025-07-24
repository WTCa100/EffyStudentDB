#include "./grade.hpp"

#include "../../utilities/inputHandler/inputHandler.hpp"

#include <iomanip>
#include <iostream>
#include <sstream>

namespace Core::Types
{
    Grade::Grade(
        uint16_t id, double value, uint16_t studentId, uint16_t subjectId, std::string studentName, std::string subjectName):
        Entry(id, g_tableGrades),
        value_(value),
        studentId_(studentId),
        subjectId_(subjectId),
        studentName_(studentName),
        subjectName_(subjectName)
    {}

    std::string Grade::toString() const
    {
        std::stringstream ss("");
        ss << id_ << ". " << studentName_ << " (" << studentId_ << ") ";
        ss << "from " << subjectName_ << " (" << subjectId_ << ") " << std::setprecision(2) << " grade: " << value_;
        return ss.str();
    }

    std::map<std::string, std::string> Grade::getAttrs() const
    {
        std::map<std::string, std::string> mappedAttrs;
        mappedAttrs.insert(std::make_pair("grade", std::to_string(value_)));
        mappedAttrs.insert(std::make_pair("subjectId", std::to_string(subjectId_)));
        mappedAttrs.insert(std::make_pair("studentId", std::to_string(studentId_)));
        return mappedAttrs;
    }

    std::unordered_map<std::string, std::string> Grade::userConstruct(bool makeFull)
    {
        std::cout << "Creating new grade from user input\n";
        std::unordered_map<std::string, std::string> mappedNewAttrs;
        int tmpHolder = makeFull ? Utilities::InputHandler::getAttrAsNumberNonEmpty("Student Id")
                                 : Utilities::InputHandler::getAttrAsNumber("Student Id");
        studentId_    = tmpHolder != g_inputMissingValue ? tmpHolder : 0;
        if (studentId_) mappedNewAttrs.insert(std::make_pair("studentId", std::to_string(studentId_)));
        tmpHolder  = makeFull ? Utilities::InputHandler::getAttrAsNumberNonEmpty("Subject Id")
                              : Utilities::InputHandler::getAttrAsNumber("Subject Id");
        subjectId_ = tmpHolder != g_inputMissingValue ? tmpHolder : 0;
        if (subjectId_) mappedNewAttrs.insert(std::make_pair("subjectId", std::to_string(subjectId_)));

        // @TODO add float handling. So far that is just a placeholder
        double tmpGrade = makeFull ? Utilities::InputHandler::getAttrAsNumberNonEmpty("Grade value")
                                   : Utilities::InputHandler::getAttrAsNumber("Grade value");
        value_          = tmpGrade != g_inputMissingValue ? tmpGrade : 0.0f;
        if (value_) mappedNewAttrs.insert(std::make_pair("grade", std::to_string(value_)));
        return mappedNewAttrs;
    }

    std::shared_ptr<Entry> Grade::mirrorMissing(const std::shared_ptr<Entry> other)
    {
        std::shared_ptr<Grade> concrete = std::static_pointer_cast<Grade>(other);
        std::shared_ptr<Grade> refObj   = std::make_shared<Grade>();

        // @TODO add function that will do the filling of a given type
        refObj->id_          = id_ == 0 ? concrete->id_ : id_;
        refObj->studentId_   = studentId_ == 0 ? concrete->studentId_ : studentId_;
        refObj->subjectId_   = subjectId_ == 0 ? concrete->subjectId_ : subjectId_;
        refObj->studentName_ = studentName_.empty() ? concrete->studentName_ : studentName_;
        refObj->subjectName_ = subjectName_.empty() ? concrete->subjectName_ : subjectName_;
        return refObj;
    }

    Entry& Grade::operator= (const Entry& other)
    {
        if (this == &other) return *this;

        const auto& otherGrade = dynamic_cast<const Grade&>(other);
        id_                    = otherGrade.id_;
        studentId_             = otherGrade.studentId_;
        subjectId_             = otherGrade.subjectId_;
        studentName_           = otherGrade.studentName_;
        subjectName_           = otherGrade.subjectName_;
        return *this;
    }
}  // namespace Core::Types
