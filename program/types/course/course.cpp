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

    void Course::userConstruct(bool makeFull)
    {
        std::cout << "Creating course from user input\n";
        minStudents_ = Utilities::InputHandler::getAttrAsNumber("ammount of minimal students");
        maxStudents_ = Utilities::InputHandler::getAttrAsNumber("ammount of maximal students");
        baseMinimalPoints_ = Utilities::InputHandler::getAttrAsNumber("Minimal points required");
        name_ = Utilities::InputHandler::getAttrAsStringNonEmpty("Name");
        return;
    }

    Course::Course(uint16_t id, uint16_t minStudents, uint16_t maxStudents, uint16_t baseMinimalPoints, std::string name) :
            Entry(id, "Courses"),
            minStudents_(minStudents),
            maxStudents_(maxStudents),
            baseMinimalPoints_(baseMinimalPoints),
            averageStudentPoints_(baseMinimalPoints),
            name_(name),
            subjectWithWeight_({})
    {}

    Course::Course(uint16_t minStudents, uint16_t maxStudents, uint16_t baseMinimalPoints , std::string name) :
            Entry(0, "Courses"),
            minStudents_(minStudents),
            maxStudents_(maxStudents),
            baseMinimalPoints_(baseMinimalPoints),
            averageStudentPoints_(baseMinimalPoints),
            name_(name),
            subjectWithWeight_({})
    {}

}