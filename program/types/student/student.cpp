#include "student.hpp"

#include "../../utilities/inputHandler/inputHandler.hpp"

#include <iostream>
#include <sstream>

namespace Core::Types
{
    std::string Student::toString(bool showGrades) const
    {
        std::stringstream ss("");
        ss << toString() << "\n";
        if (showGrades)
        {
            if (!grades_.empty())
            {
                ss << ", Grades: \n";
                for (const auto& grade : grades_) { ss << grade.second->studentName_ << ":" << grade.second->value_ << "\n"; }
            }
            else { ss << "No grades.\n"; }
        }
        return ss.str();
    }

    std::string Student::toString() const
    {
        std::stringstream ss("");
        ss << "Id: " << id_ << ", First name: " << firstName_ << ", ";
        if (secondName_.has_value() && !secondName_.value().empty()) { ss << "Second name: " << secondName_.value() << ", "; }
        ss << "Last name: " << lastName_ << ", ";
        ss << "Email: " << email_ << ", ";
        ss << "Associated School: " << schoolId_ << " ";
        if (!attendingCourses_.empty())
        {
            ss << "Attends " << attendingCourses_.size() << " courses: \n";
            for (const auto& course : attendingCourses_) { ss << course.second << " "; }
        }
        else { ss << "No attending courses."; }
        return ss.str();
    }

    std::map<std::string, std::string> Student::getAttrs() const
    {
        std::map<std::string, std::string> mappedAttrs;
        mappedAttrs.insert(std::make_pair("firstName", firstName_));
        if (secondName_.has_value()) mappedAttrs.insert(std::make_pair("secondName", secondName_.value()));
        mappedAttrs.insert(std::make_pair("lastName", lastName_));
        mappedAttrs.insert(std::make_pair("email", email_));
        mappedAttrs.insert(std::make_pair("schoolId", std::to_string(schoolId_)));
        return mappedAttrs;
    }

    std::unordered_map<std::string, std::string> Student::userConstruct(bool makeFull)
    {
        std::unordered_map<std::string, std::string> mappedNewAttrs;
        std::cout << "Creating student from user input\n";
        std::cout << "(Attributes tagged as 'optional' can be left empty)\n";
        firstName_ = makeFull ? Utilities::InputHandler::getAttrAsStringNonEmpty("First name")
                              : Utilities::InputHandler::getAttrAsString("First name");
        if (!firstName_.empty()) mappedNewAttrs.insert(std::make_pair("firstName", firstName_));

        secondName_ = Utilities::InputHandler::getAttrAsString("Second name (optional)");
        if (secondName_.has_value() && secondName_.value().empty()) { secondName_ = std::nullopt; }

        lastName_ = makeFull ? Utilities::InputHandler::getAttrAsStringNonEmpty("Last name")
                             : Utilities::InputHandler::getAttrAsString("Last name");
        if (!lastName_.empty()) mappedNewAttrs.insert(std::make_pair("lastName", lastName_));

        email_ = makeFull ? Utilities::InputHandler::getAttrAsStringNonEmpty("Email")
                          : Utilities::InputHandler::getAttrAsString("Email");
        if (!email_.empty()) mappedNewAttrs.insert(std::make_pair("email", email_));

        int tmpSchoolId = makeFull ? Utilities::InputHandler::getAttrAsNumberNonEmpty("Associated School (ID)")
                                   : Utilities::InputHandler::getAttrAsNumber("Associated School (ID)");
        schoolId_       = tmpSchoolId != g_inputMissingValue ? tmpSchoolId : 0;
        if (schoolId_ != g_inputMissingValue) mappedNewAttrs.insert(std::make_pair("schoolId", std::to_string(schoolId_)));

        return mappedNewAttrs;
    }

    std::shared_ptr<Entry> Student::mirrorMissing(const std::shared_ptr<Entry> other)
    {
        std::shared_ptr<Student> concrete = std::static_pointer_cast<Student>(other);
        std::shared_ptr<Student> refObj   = std::make_shared<Student>();
        refObj->id_                       = id_ == 0 ? concrete->id_ : id_;
        refObj->firstName_                = firstName_.empty() ? concrete->firstName_ : firstName_;
        if (concrete->secondName_.has_value())
        {
            if (secondName_.has_value())
            {
                refObj->secondName_ = secondName_.value().empty() ? concrete->secondName_ : secondName_;
            }
            else { refObj->secondName_ = concrete->secondName_; }
        }
        else
        {
            if (secondName_.has_value()) { refObj->secondName_ = concrete->secondName_; }
            // No else as by default secondName_ is set to nullopt
        }
        refObj->lastName_ = lastName_.empty() ? concrete->lastName_ : lastName_;
        refObj->email_    = email_.empty() ? concrete->email_ : email_;
        refObj->schoolId_ = schoolId_ == 0 ? concrete->schoolId_ : schoolId_;
        return refObj;
    }

    Student::Student(uint16_t id,
        std::string name,
        std::string lastName,
        std::string email,
        uint16_t schoolId,
        std::optional<std::string> secondName):
        Entry(id, g_tableStudents),
        firstName_(name),
        secondName_(secondName),
        lastName_(lastName),
        email_(email),
        grades_({}),
        schoolId_(schoolId)
    {}

    Student::Student(
        std::string name, std::string lastName, std::string email, uint16_t schoolId, std::optional<std::string> secondName):
        Entry(0, g_tableStudents),
        firstName_(name),
        secondName_(secondName),
        lastName_(lastName),
        email_(email),
        grades_({}),
        schoolId_(schoolId)
    {}

    Entry& Student::operator= (const Entry& other)
    {
        if (this == &other) return *this;

        const auto& otherStudent = dynamic_cast<const Student&>(other);
        id_                      = other.id_;
        firstName_               = otherStudent.firstName_;
        secondName_              = otherStudent.secondName_;
        lastName_                = otherStudent.lastName_;
        email_                   = otherStudent.email_;
        grades_                  = otherStudent.grades_;
        schoolId_                = otherStudent.schoolId_;

        return *this;
    }

}  // namespace Core::Types
