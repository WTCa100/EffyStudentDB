#include "session.hpp"

#include "../../utilities/common/stringManip.hpp"

Session::Session(std::shared_ptr<WsManager> wsMgr) : wsMgr_(wsMgr),
                                                     logger_(wsMgr_->getLogger()),
                                                     sAdapter_(std::make_unique<SqlAdapter>(logger_, wsMgr_->getSqlManager())),
                                                     sesData_(std::make_shared<SessionData>()),
                                                     display_(std::make_unique<Menu>(logger_, sesData_))
{
    LOG((*logger_), "Loading initial database entries");
    fetchAll();

    School mockSchool = School{0, "Non_Existing_School", {}};
    addSchool(mockSchool);

    Subject mockSubject = Subject{0, "Non_Existing_Subject"};
    addSubject(mockSubject);

    Student mockStudent = Student{0, "John", std::nullopt, "Doe", "john.doe@mail.com", {}, mockSchool.id_};
    addStudent(mockStudent);

    Course mockCourse = Course{0, 20, 50, 20, 0, "Invalid_Course", {}};
    addCourse(mockCourse);

    sesData_->showSchools();
    sesData_->showCourses();

    School newMockSchool = School{0, "Existing_School", {}};
    Course newMockCourse = Course{0, 10, 40, 10, 0, "Valid_Course", {}};
    updateSchool(mockSchool, newMockSchool);
    updateCourse(mockCourse, newMockCourse);
    sesData_->showSchools();
    sesData_->showCourses();
    sesData_->showStudentRequests(true);
    
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
        sesData_->addSchool(entry);
    }
}

void Session::fetchStudents()
{
    LOG((*logger_), "Fetching students");
    std::vector<Core::Types::Student> dbStudents = sAdapter_->getStudents();
    LOG((*logger_), "Got ", dbStudents.size() , " Entries");
    for(const auto& entry : dbStudents)
    {
        sesData_->addStudent(entry);
    }
}

void Session::fetchSubjects()
{
    LOG((*logger_), "Fetching subjects");
    std::vector<Core::Types::Subject> dbSubjects = sAdapter_->getSubjects();
    LOG((*logger_), "Got ", dbSubjects.size() , " Entries");
    for(const auto& entry : dbSubjects)
    {
        sesData_->addSubject(entry);
    }
}

void Session::fetchCourses()
{
    LOG((*logger_), "Fetching courses");
    std::vector<Core::Types::Course> dbCourses = sAdapter_->getCourses();
    for(const auto& entry : dbCourses)
    {
        sesData_->addCourse(entry);
    }
}

void Session::fetchSrequests()
{
    LOG((*logger_), "Fetching student requests");
    std::vector<Core::Types::Request::Srequest> dbSrequests = sAdapter_->getSrequests();
    for(const auto& entry : dbSrequests)
    {
        sesData_->addStudentRequest(entry);
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
                        std::cout << "Failed to execute the command.";
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
    
    // 1st Table 2nd action 3rd (optional) targetId
    std::string table = tokenizedCommand.at(0);
    std::string action = tokenizedCommand.at(1);
    std::optional<std::string> targetId = (tokenizedCommand.size() == 3) ? std::optional<std::string>(tokenizedCommand.at(2)) : std::nullopt;

    if(table == "SCHOOL") 
    {
        if(action == "ADD")
        {
            // Construct school
            School newSchool = display_->constructSchool();
            return addSchool(newSchool);
        }
    }
    else if(table == "STUDENT") {}
    else if(table == "SUBJECT") {}
    else if (table == "COURSE") {}
    else if (table == "SREQUEST") {}
    return true;
}

bool Session::addSchool(School& newSchool)
{
    LOG((*logger_), "Adding new school: \"", newSchool.name_, "\"");
    if(sAdapter_->addSchool(newSchool))
    {
        sesData_->addSchool(newSchool);
        return true;
    }
    return false;
}

bool Session::updateSchool(School& targetSchool, School& newSchool)
{
    LOG((*logger_), "Editing existing school");
    if(sAdapter_->updateSchool(targetSchool, newSchool))
    {
        // This will be moved to the in-menu editing screen later on
        targetSchool.name_ = newSchool.name_;
        // <- Untill this moment
        sesData_->updateSchool(targetSchool.id_, newSchool);
        return true;
    }
    return false;
}

bool Session::removeSchool(School targetSchool)
{
    LOG((*logger_), "Removing existing school: \"", targetSchool.name_, "\"");
    if(sAdapter_->removeSchool(targetSchool))
    {
        sesData_->removeSchool(targetSchool.id_);
        return true;
    }
    LOG((*logger_), "No such school present!");
    return false;
}

bool Session::addStudent(Student& newStudent)
{
    LOG((*logger_), "Adding new student: {", newStudent.firstName_, " ", newStudent.lastName_, " ", newStudent.email_, "}");
    if(sAdapter_->addStudent(newStudent))
    {
        sesData_->addStudent(newStudent);
        return true;
    }
    return false;
}

bool Session::removeStudent(Student targetStudent)
{
    LOG((*logger_), "Removing student: id=", targetStudent.id_);
    if(sAdapter_->removeStudent(targetStudent))
    {
        sesData_->removeStudent(targetStudent);
        return true;
    }
    return false;
}

bool Session::addSubject(Subject& targetSubject)
{
    LOG((*logger_), "Adding new subject ", targetSubject.name_);
    if(sAdapter_->addSubject(targetSubject))
    {
        sesData_->addSubject(targetSubject);
        return true;
    }
    return false;
}

bool Session::removeSubject(Subject targetSubject)
{
    LOG((*logger_), "Attempting to remove subject ", targetSubject.name_);
    if(sAdapter_->removeSubject(targetSubject))
    {
        sesData_->removeSubject(targetSubject);
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
    if(sAdapter_->addCourse(newCourse))
    {
        sesData_->addCourse(newCourse);
        return true;
    }
    return false;
}

bool Session::updateCourse(Course& targetCourse, Course& newCourse)
{
    LOG((*logger_), "Updating existing course.");
    if(sAdapter_->updateCourse(targetCourse, newCourse))
    {
        sesData_->updateCourse(targetCourse.id_, newCourse);
        return true;
    }
    return false;
}

bool Session::removeCourse(Course targetCourse)
{
    LOG((*logger_), "Removing course: ", targetCourse.name_);
    return false;
}

bool Session::addSrequest(Srequest& newSrequest)
{
    return false;
}

bool Session::removeSrequest(Srequest targetSrequest)
{

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