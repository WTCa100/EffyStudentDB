#include "course.hpp"

#include <sstream>
#include <iostream>

#include "../../utilities/inputHandler/inputHandler.hpp"
namespace Core::Types
{
    std::string Course::toString() const
    {
        std::stringstream ss("");
        ss << "id: " << id_ << " name: " << name_ << " minimal student count: " << minStudents_ << " maximal student count: " << maxStudents_ << "\n"
           << "minimal points required: " << baseMinimalPoints_ << " average student points: " << averageStudentPoints_;
        return ss.str();
    }

    std::map<std::string, std::string> Course::getAttrs() const
    {
        return  {
            {"minStudents", std::to_string(minStudents_)},
            {"maxStudents", std::to_string(maxStudents_)},
            {"baseMinimalPoints", std::to_string(baseMinimalPoints_)},
            {"name", name_}
        };
    }

    std::unordered_map<std::string, std::string> Course::userConstruct(bool makeFull)
    {
        std::cout << "Creating course from user input\n";
        minStudents_ = Utilities::InputHandler::getAttrAsNumberNonEmpty("Ammount of minimal students");
        maxStudents_ = Utilities::InputHandler::getAttrAsNumberNonEmpty("Ammount of maximal students");
        baseMinimalPoints_ = Utilities::InputHandler::getAttrAsNumberNonEmpty("Minimal points required");
        name_ = Utilities::InputHandler::getAttrAsStringNonEmpty("Name");
        return {};
    }

    Course::Course(uint16_t id, uint16_t minStudents, uint16_t maxStudents, uint16_t baseMinimalPoints, std::string name) :
            Entry(id, g_tableCourses),
            subjectWithWeight_({}),
            name_(name),
            minStudents_(minStudents),
            maxStudents_(maxStudents),
            baseMinimalPoints_(baseMinimalPoints),
            averageStudentPoints_(baseMinimalPoints)
    {}

    Course::Course(uint16_t minStudents, uint16_t maxStudents, uint16_t baseMinimalPoints , std::string name) :
            Entry(0, g_tableCourses),
            subjectWithWeight_({}),
            name_(name),
            minStudents_(minStudents),
            maxStudents_(maxStudents),
            baseMinimalPoints_(baseMinimalPoints),
            averageStudentPoints_(baseMinimalPoints)
    {}

    Entry& Course::operator=(const Entry& other)
    {
        if(this == &other) return *this;

        const auto& otherCourse = dynamic_cast<const Course&>(other);
        
        id_                   = otherCourse.id_;
        subjectWithWeight_    = otherCourse.subjectWithWeight_;
        name_                 = otherCourse.name_;
        minStudents_          = otherCourse.minStudents_;
        maxStudents_          = otherCourse.maxStudents_;
        baseMinimalPoints_    = otherCourse.baseMinimalPoints_;
        averageStudentPoints_ = otherCourse.averageStudentPoints_;
        return *this;
    }


}