#include "course.hpp"

#include "../../utilities/inputHandler/inputHandler.hpp"

#include <iostream>
#include <sstream>

namespace Core::Types
{
    std::string Course::toString() const
    {
        std::stringstream ss("");
        ss << "id: " << id_ << " name: " << name_ << " minimal student count: " << minStudents_
           << " maximal student count: " << maxStudents_ << "\n"
           << "minimal points required: " << baseMinimalPoints_ << " average student points: " << averageStudentPoints_ << "\n"
           << "Is open for assignment: " << (isOpen_ == OpenState::opened ? "Yes" : "No")
           << " - current recuritment turn: " << recrutingTurn_;
        return ss.str();
    }

    std::map<std::string, std::string> Course::getAttrs() const
    {
        return {
            { "minStudents",       std::to_string(minStudents_)                  },
            { "maxStudents",       std::to_string(maxStudents_)                  },
            { "baseMinimalPoints", std::to_string(baseMinimalPoints_)            },
            { "name",              name_                                         },
            { "isOpen",            std::to_string(static_cast<uint8_t>(isOpen_)) },
            { "recrutingTurn",     std::to_string(recrutingTurn_)                }
        };
    }

    std::unordered_map<std::string, std::string> Course::userConstruct(bool makeFull)
    {
        std::unordered_map<std::string, std::string> mappedNewAttrs;
        std::cout << "Creating course from user input\n";
        name_ = makeFull ? Utilities::InputHandler::getAttrAsStringNonEmpty("Name")
                         : Utilities::InputHandler::getAttrAsString("Name");
        if (!name_.empty()) mappedNewAttrs.insert(std::make_pair("name", name_));

        baseMinimalPoints_ = makeFull ? Utilities::InputHandler::getAttrAsNumberNonEmpty("Minimal points required")
                                      : Utilities::InputHandler::getAttrAsNumber("Minimal points required");
        if (baseMinimalPoints_ != g_inputMissingValue)
            mappedNewAttrs.insert(std::make_pair("baseMinimalPoints", std::to_string(baseMinimalPoints_)));

        minStudents_ = makeFull ? Utilities::InputHandler::getAttrAsNumberNonEmpty("Ammount of minimal students")
                                : Utilities::InputHandler::getAttrAsNumber("Ammount of minimal students");
        if (minStudents_ != g_inputMissingValue)
            mappedNewAttrs.insert(std::make_pair("minStudents", std::to_string(minStudents_)));
        do {
            maxStudents_ = makeFull ? Utilities::InputHandler::getAttrAsNumberNonEmpty("Ammount of maximal students")
                                    : Utilities::InputHandler::getAttrAsNumber("Ammount of maximal students");
            if (maxStudents_ < minStudents_ && !makeFull) { std::cout << "Max students cannot be lesser than min students!\n"; }
        } while (maxStudents_ < minStudents_);
        if (maxStudents_ != g_inputMissingValue)
            mappedNewAttrs.insert(std::make_pair("maxStudents", std::to_string(maxStudents_)));

        // Any freshly added course will be opened from the get-go
        isOpen_        = OpenState::opened;
        recrutingTurn_ = 1;
        mappedNewAttrs.insert(std::make_pair("isOpen", std::to_string(static_cast<uint8_t>(isOpen_))));
        mappedNewAttrs.insert(std::make_pair("recrutingTurn", std::to_string(recrutingTurn_)));

        return mappedNewAttrs;
    }

    std::shared_ptr<Entry> Course::mirrorMissing(const std::shared_ptr<Entry> other)
    {
        std::shared_ptr<Course> concrete = std::static_pointer_cast<Course>(other);
        std::shared_ptr<Course> retObj   = std::make_shared<Course>();
        retObj->id_                      = id_ == 0 ? concrete->id_ : id_;
        retObj->name_                    = name_.empty() ? concrete->name_ : name_;
        retObj->minStudents_             = minStudents_ == g_inputMissingValue ? concrete->minStudents_ : minStudents_;
        retObj->maxStudents_             = maxStudents_ == g_inputMissingValue ? concrete->maxStudents_ : maxStudents_;
        retObj->baseMinimalPoints_ =
            baseMinimalPoints_ == g_inputMissingValue ? concrete->baseMinimalPoints_ : baseMinimalPoints_;
        retObj->averageStudentPoints_ =
            averageStudentPoints_ == g_inputMissingValue ? concrete->averageStudentPoints_ : averageStudentPoints_;
        retObj->isOpen_        = isOpen_ == OpenState::notSet ? concrete->isOpen_ : isOpen_;
        retObj->recrutingTurn_ = recrutingTurn_ == turnNotSet ? concrete->recrutingTurn_ : recrutingTurn_;
        return retObj;
    }

    Course::Course():
        Entry(0, g_tableCourses),
        subjectWithWeight_({}),
        name_(""),
        minStudents_(0),
        maxStudents_(0),
        baseMinimalPoints_(0),
        averageStudentPoints_(0),
        isOpen_(OpenState::notSet),
        recrutingTurn_(turnNotSet)
    {}

    Course::Course(uint16_t id,
        uint16_t minStudents,
        uint16_t maxStudents,
        uint16_t baseMinimalPoints,
        std::string name,
        uint8_t isOpen,
        uint16_t recrutingTurn):
        Entry(id, g_tableCourses),
        subjectWithWeight_({}),
        name_(name),
        minStudents_(minStudents),
        maxStudents_(maxStudents),
        baseMinimalPoints_(baseMinimalPoints),
        averageStudentPoints_(baseMinimalPoints),
        isOpen_(static_cast<OpenState>(isOpen)),
        recrutingTurn_(recrutingTurn)
    {}

    Course::Course(uint16_t minStudents, uint16_t maxStudents, uint16_t baseMinimalPoints, std::string name):
        Entry(0, g_tableCourses),
        subjectWithWeight_({}),
        name_(name),
        minStudents_(minStudents),
        maxStudents_(maxStudents),
        baseMinimalPoints_(baseMinimalPoints),
        averageStudentPoints_(baseMinimalPoints),
        isOpen_(OpenState::notSet),
        recrutingTurn_(turnNotSet)
    {}

    Entry& Course::operator= (const Entry& other)
    {
        if (this == &other) return *this;

        const auto& otherCourse = dynamic_cast<const Course&>(other);

        id_                   = otherCourse.id_;
        subjectWithWeight_    = otherCourse.subjectWithWeight_;
        name_                 = otherCourse.name_;
        minStudents_          = otherCourse.minStudents_;
        maxStudents_          = otherCourse.maxStudents_;
        baseMinimalPoints_    = otherCourse.baseMinimalPoints_;
        averageStudentPoints_ = otherCourse.averageStudentPoints_;
        isOpen_               = otherCourse.isOpen_;
        recrutingTurn_        = otherCourse.recrutingTurn_;
        return *this;
    }

}  // namespace Core::Types

