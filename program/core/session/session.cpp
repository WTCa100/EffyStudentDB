#include "session.hpp"

#include "../../utilities/common/stringManip.hpp"

#include "../../types/entry.hpp"

Session::Session(std::shared_ptr<WsManager> wsMgr) : wsMgr_(wsMgr),
                                                     logger_(wsMgr_->getLogger()),
                                                     sAdapter_(std::make_unique<SqlAdapter>(logger_, wsMgr_->getSqlManager())),
                                                     sesData_(std::make_shared<SessionData>()),
                                                     display_(std::make_unique<Menu>(logger_, sesData_))
{
    LOG((*logger_), "Loading initial database entries");
    fetchAll();
    LOG((*logger_), "Session established");
}

void Session::fetchAll()
{
    LOG((*logger_), "Fetching all entries from the database");
    fetchSchools();
    fetchStudents();
    fetchSubjects();
    fetchGrades();
    fetchCourses();
    fetchSrequests();
    LOG((*logger_), "Fetching done");
}

void Session::fetchSchools()
{
    LOG((*logger_), "Fetching schools");
    std::vector<Core::Types::School> dbSchools = sAdapter_->getSchools();
    LOG((*logger_), "Got ", dbSchools.size() , " Entries");
    for(const auto& entry : dbSchools)
    {
        sesData_->addEntry(std::make_shared<School>(entry));
    }
}

void Session::fetchStudents()
{
    LOG((*logger_), "Fetching students");
    std::vector<Core::Types::Student> dbStudents = sAdapter_->getStudents();
    LOG((*logger_), "Got ", dbStudents.size() , " Entries");
    for(const auto& entry : dbStudents)
    {
        sesData_->addEntry(std::make_shared<Student>(entry));

        const uint16_t schoolId = entry.schoolId_;
        std::shared_ptr<School> targetSchool = std::dynamic_pointer_cast<School>(sesData_->getEntry(schoolId, "Schools"));
        if(targetSchool)
        {
            (*targetSchool).students_.insert(std::make_pair(entry.id_, entry));
        }
        else
        {
            std::runtime_error("Could not fetch school data with the following data: ID=" + std::to_string(schoolId));
        }

    }
}

void Session::fetchSubjects()
{
    LOG((*logger_), "Fetching subjects");
    std::vector<Core::Types::Subject> dbSubjects = sAdapter_->getSubjects();
    LOG((*logger_), "Got ", dbSubjects.size() , " Entries");
    for(const auto& entry : dbSubjects)
    {
        sesData_->addEntry(std::make_shared<Subject>(entry));
    }
}

void Session::fetchCourses()
{
    LOG((*logger_), "Fetching courses");
    std::vector<Core::Types::Course> dbCourses = sAdapter_->getCourses();
    for(const auto& entry : dbCourses)
    {
        sesData_->addEntry(std::make_shared<Course>(entry));
    }
}

void Session::fetchSrequests()
{
    LOG((*logger_), "Fetching student requests");
    std::vector<Core::Types::Request::Srequest> dbSrequests = sAdapter_->getSrequests();
    for(const auto& entry : dbSrequests)
    {
        sesData_->addEntry(std::make_shared<Srequest>(entry));
    }
}

void Session::run()
{
    LOG((*logger_), "Main run function called");
    bool exit = false;
    do
    {
        Core::Display::MainMenuOption op = display_->showMainMenu();
        switch (op)
        {
        case Core::Display::MainMenuOption::manageDb:
            {
                std::string command = "";
                do
                {
                    command = display_->manageDatabase();
                    if(executeCommand(command))
                    {
                        std::cout << "Done!\n";
                    }
                    else
                    {
                        std::cout << "Failed to execute the command.\n";
                    }
                } while (command != "EXIT");
                
                break;
            }
        case Core::Display::MainMenuOption::handleRqs:
            break;
        case Core::Display::MainMenuOption::exit:
            exit = true;
            LOG((*logger_), "Exiting application");
        default:
            break;
        }
        
    } while (!exit);

}

bool Session::executeCommand(std::string command)
{
    LOG((*logger_), "Executing session command: ", command);
    std::cout << "DBG: Command = " << command << "\n";
    if(command == "EXIT") return true;

    std::vector<std::string> tokenizedCommand = Utilities::Common::tokenize(command , ' ');
    
    // 1st Table 2nd action 3rd (optional - used only for editing and removing) targetId
    std::string table = tokenizedCommand.at(0);
    std::string action = tokenizedCommand.at(1);
    std::optional<std::string> targetId = (tokenizedCommand.size() == 3) ? std::optional<std::string>(tokenizedCommand.at(2)) : std::nullopt;

    // Consider moving it into a map of lambdas
    // like: std::unordered_map<std::string, std::function<bool()>> commandMap;

    if(table == "SCHOOL") 
    {
        if(action == "ADD")
        {
            School newSchool;
            newSchool.userConstruct(true);
            return addSchool(newSchool);
        }
        
        School targetSchool;
        std::vector<School> affectedEntries;
        bool handledAllEntries = true;
        if(!targetId.has_value())
        {
           targetSchool.userConstruct(true); // only one attr present it has to be passed
           affectedEntries = sAdapter_->getSchools("name = " + targetSchool.name_);
        }
        else
        {
            targetSchool.id_ = std::stoul(targetId.value());
            affectedEntries = sAdapter_->getSchools("id = " + targetId.value());
        }

        if(affectedEntries.empty())
        {
            LOG((*logger_), "No such school present!");
            return false;
        }

        if(action == "ALTER")
        {
            School alteredSchool;
            std::cout << "Input new school data\n";
            alteredSchool.userConstruct();
            // TODO: Ask user if he wants to update all of the entries
            std::cout << "Updating school: " << affectedEntries.at(0).name_ << "\n";
            if(affectedEntries.size() > 1)
            {
                std::cout << "And " << affectedEntries.size() - 1 << " more entries\n";
            }
            for(auto& entry : affectedEntries)
            {
                if(!updateSchool(entry, alteredSchool))
                {
                    LOG((*logger_), "Failed to update school: ", entry.name_);
                    std::cout << "Failed to update school: " << entry.name_ << "\n";
                    handledAllEntries = false;
                }
            }
            return handledAllEntries;
        }

        if(action == "REMOVE")
        {
            // TODO: Ask user if he wants to update all of the entries
            std::cout << "Updating school: " << affectedEntries.at(0).name_ << "\n";
            for(auto& entry : affectedEntries)
            {
                if(!removeSchool(targetSchool))
                {
                    LOG((*logger_), "Failed to remove school: ", entry.name_);
                    std::cout << "Failed to remove school: " << entry.name_ << "\n";
                    handledAllEntries = false;
                }
            }
            return handledAllEntries;
        }

        // Find?

    }
    else if(table == "STUDENT") 
    {
        if(action == "ADD")
        {
            Student newStudent;
            newStudent.userConstruct(true);
            return addStudent(newStudent);
        }

        Student targetStudent;
        std::vector<Student> affectedEntries;
        bool handledAllEntries = true;
        if(!targetId.has_value())
        {
            std::unordered_map<std::string, std::string> providedAttrs = targetStudent.userConstruct(true); // only one attr present it has to be passed
            std::string filter = sAdapter_->makeFilter(providedAttrs);
            affectedEntries = sAdapter_->getStudents(filter);
        }
        else
        {
            targetStudent.id_ = std::stoul(targetId.value());
            affectedEntries = sAdapter_->getStudents("id = " + targetId.value());
        }

        if(affectedEntries.empty())
        {
            LOG((*logger_), "No such student present!");
            return false;
        }

        if(action == "REMOVE")
        {
            // TODO: Ask user if he wants to update all of the entries
            std::cout << "Updating students: " << affectedEntries.at(0).firstName_ << "\n";
            for(auto& entry : affectedEntries)
            {
                if(!removeStudent(targetStudent))
                {
                    LOG((*logger_), "Failed to remove student: ", entry.firstName_);
                    std::cout << "Failed to remove student: " << entry.firstName_ << "\n";
                    handledAllEntries = false;
                }
            }
            return handledAllEntries;
        }
    }
    else if(table == "SUBJECT") {}
    else if (table == "COURSE") {}
    else if (table == "SREQUEST") {}
    return true;
}

bool Session::addSchool(School& newSchool)
{
    LOG((*logger_), "Adding new school: \"", newSchool.name_, "\"");
    if(sAdapter_->addEntry(newSchool))
    {
        sesData_->addEntry(std::make_shared<School>(newSchool));
        return true;
    }
    return false;
}

bool Session::updateSchool(School& targetSchool, School& newSchool)
{
    LOG((*logger_), "Editing existing school");
    if(sAdapter_->updateEntry(targetSchool, newSchool))
    {
        sesData_->updateEntry(targetSchool.id_, std::make_shared<School>(newSchool));
        return true;
    }
    return false;
}

bool Session::removeSchool(School targetSchool)
{
    LOG((*logger_), "Removing existing school: \"", targetSchool.name_, "\"");
    if(sAdapter_->removeEntry(targetSchool))
    {
        sesData_->removeEntry(targetSchool.id_, targetSchool.associatedTable_);
        return true;
    }
    LOG((*logger_), "No such school present!");
    return false;
}

bool Session::addStudent(Student& newStudent)
{
    LOG((*logger_), "Adding new student: {", newStudent.firstName_, " ", newStudent.lastName_, " ", newStudent.email_, "}");
    
    const uint16_t& newStudentSchoolId = newStudent.schoolId_;

    if(!sesData_->isPresent(newStudentSchoolId, "Schools"))
    {
        LOG((*logger_), "WARN! Attempted to add student to non existing school!");
        std::cout << "No school with id " << newStudentSchoolId << " exists!\n";
        return false;
    }

    if(sAdapter_->addEntry(newStudent))
    {
        sesData_->addEntry(std::make_shared<Student>(newStudent));
        // Link student with a school
        std::shared_ptr<School> targetSchool = std::dynamic_pointer_cast<School>(sesData_->getEntry(newStudentSchoolId, "Schools"));
        if(targetSchool)
        {
            (*targetSchool).students_.insert(std::make_pair(newStudent.id_, newStudent));
        }
        else
        {
            std::runtime_error("Could not fetch school data with the following data: ID=" + std::to_string(newStudentSchoolId));
        }
        return true;
    }
    return false;
}

bool Session::removeStudent(Student targetStudent)
{
    const uint16_t& studentId = targetStudent.id_;
    LOG((*logger_), "Removing student: id=", studentId);
    if(sAdapter_->removeEntry(targetStudent))
    {
        // Get entry to remove linked school
        const int16_t linkedSchool = std::dynamic_pointer_cast<Student>(sesData_->getEntry(studentId, "Students"))->schoolId_;
        LOG((*logger_), "Student Id = ", studentId, " has school id assinged = " , linkedSchool)
        sesData_->removeEntry(studentId, targetStudent.associatedTable_);
        // Look for associated school
        std::shared_ptr<School> associatedSchool = std::dynamic_pointer_cast<School>(sesData_->getEntry(linkedSchool, "Schools"));
        if(associatedSchool)
        {
            (*associatedSchool).students_.erase(studentId);
            LOG((*logger_), "Deleted student from the school students list");
        }
        else
        {
            std::runtime_error("Could not fetch school data with the following data: ID=" + std::to_string(targetStudent.schoolId_));
        }
        return true;
    }
    return false;
}

bool Session::addSubject(Subject& targetSubject)
{
    LOG((*logger_), "Adding new subject ", targetSubject.name_);
    if(sAdapter_->addEntry(targetSubject))
    {
        sesData_->addEntry(std::make_shared<Subject>(targetSubject));
        return true;
    }
    return false;
}

bool Session::removeSubject(Subject targetSubject)
{
    LOG((*logger_), "Attempting to remove subject ", targetSubject.name_);
    if(sAdapter_->removeEntry(targetSubject))
    {
        sesData_->removeEntry(targetSubject.id_, targetSubject.associatedTable_);

        // TODO: Loop over courses, scores & grades , and delete ones with given subject
        return true;
    }
    return false;
}

bool Session::addGrade(Subject& targetSubject, Student& targetStudent, float value)
{
    LOG((*logger_), "Adding new grade for pair: ", targetSubject.name_, "-", targetStudent.email_, " value=", value);
    if(sAdapter_->addGrade(targetStudent, targetSubject, value))
    {
        sesData_->addGrade(targetSubject.id_, targetStudent.id_, value);
        return true;
    }
    return false;
}

bool Session::removeGrade(Subject targetSubject, Student targetStudent)
{
    LOG((*logger_), "Removing grade for pair: ", targetSubject.name_, "-", targetStudent.email_);
    if(sAdapter_->removeGrade(targetStudent, targetSubject))
    {
        sesData_->removeGrade(targetSubject.id_, targetSubject.id_);
        return true;
    }
    return false;
}

bool Session::addCourse(Course& newCourse)
{
    LOG((*logger_), "Adding new course: ", newCourse.name_);
    if(sAdapter_->addEntry(newCourse))
    {
        sesData_->addEntry(std::make_shared<Course>(newCourse));
        return true;
    }
    return false;
}

bool Session::updateCourse(Course& targetCourse, Course& newCourse)
{
    LOG((*logger_), "Updating existing course.");
    if(sAdapter_->updateEntry(targetCourse, newCourse))
    {
        sesData_->updateEntry(targetCourse.id_, std::make_shared<Course>(newCourse));
        return true;
    }
    return false;
}

bool Session::removeCourse(Course targetCourse)
{
    LOG((*logger_), "Removing course: ", targetCourse.name_);
    if(sAdapter_->removeEntry(targetCourse))
    {
        sesData_->removeEntry(targetCourse.id_, targetCourse.associatedTable_);

        // TODO Do we need a handle to subject weights?
        return true;
    }
    return false;
}

bool Session::addSrequest(Srequest& newSrequest)
{
    LOG((*logger_), "Add student request: from: ", newSrequest.studentId_, " to: ", newSrequest.courseId_);
    if(sAdapter_->addEntry(newSrequest))
    {
        sesData_->addEntry(std::make_shared<Srequest>(newSrequest));
        return true;
    }
    return false;
}

bool Session::removeSrequest(Srequest targetSrequest)
{
    LOG((*logger_), "Remove student request: from: ", targetSrequest.studentId_, " to: ", targetSrequest.courseId_);
    if(sAdapter_->removeEntry(targetSrequest))
    {
        sesData_->removeEntry(targetSrequest.id_, targetSrequest.associatedTable_);
        return true;
    }
    return false;
}

void Session::fetchGrades()
{
    LOG((*logger_), "Fetching grades");
    std::vector<std::vector<std::string>> dbGrades = sAdapter_->getGrades();
    LOG((*logger_), "Got ", dbGrades.size() , " Entries");
    if(!dbGrades.empty())
    {
        LOG((*logger_), "Linking grades")
        for(auto& entry : dbGrades)
        {
            // (0) GRADE | (1) SUBID | (2) STUDENTID | (3) ID
            float currentGrade = std::stof(entry.at(0));
            uint16_t targetedStudent = std::stoul(entry.at(1));
            uint16_t targetedSubject = std::stoul(entry.at(2));
            sesData_->addGrade(targetedSubject, targetedStudent, currentGrade);
        }
    }
}

Session::~Session()
{
    LOG((*logger_), "Session teardown!");
}