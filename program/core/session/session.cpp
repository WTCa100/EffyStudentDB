#include "session.hpp"

Session::Session(std::shared_ptr<WsManager> wsMgr, std::shared_ptr<Logger> logger) : wsMgr_(wsMgr), logger_(logger), sesData_(std::make_shared<SessionData>()), display_(std::make_unique<Menu>(logger_))
{
    LOG((*logger_), "Loading initial database entries");
    fetchAll();
    School test{0, "Test School"};
    addSchool(test);
    test = School{0, "Test School 2"};
    addSchool(test);
    test = School{0, "ABDCDSDC"};
    addSchool(test);

    sesData_->showSchools();

    test = School{1, "Whatever"};
    removeSchool(test);


    fetchSchools();
    sesData_->showSchools();
    test = School{0, "Whatever"};
    addSchool(test);
    sesData_->showSchools();
}

void Session::fetchAll()
{
    LOG((*logger_), "Fetching all entries from the database");
    fetchSchools();
    fetchStudents();
    fetchSubjects();
    fetchGrades();
    LOG((*logger_), "Fetching done");

    sesData_->showSchools();
    sesData_->showSubjects();
    sesData_->showStudents();
    sesData_->showGrades();
}

void Session::fetchSchools()
{
    LOG((*logger_), "Fetching schools");
    std::vector<Core::Types::School> dbSchools = wsMgr_->getSchools();
    LOG((*logger_), "Got ", dbSchools.size() , " Entries");
    for(const auto& entry : dbSchools)
    {
        sesData_->addSchool(entry);
    }
}

void Session::fetchStudents()
{
    LOG((*logger_), "Fetching students");
    std::vector<Core::Types::Student> dbStudents = wsMgr_->getStudents();
    LOG((*logger_), "Got ", dbStudents.size() , " Entries");
    for(const auto& entry : dbStudents)
    {
        sesData_->addStudent(entry);
    }
}

void Session::fetchSubjects()
{
    LOG((*logger_), "Fetching subjects");
    std::vector<Core::Types::Subject> dbSubjects = wsMgr_->getSubjects();
    LOG((*logger_), "Got ", dbSubjects.size() , " Entries");
    for(const auto& entry : dbSubjects)
    {
        sesData_->addSubject(entry);
    }
}

void Session::run()
{
    LOG((*logger_), "Main run function called");
    Core::Display::MainMenuOption op = display_->showMainMenu();
    switch (op)
    {
    case Core::Display::MainMenuOption::manageDb:
        
        break;
    case Core::Display::MainMenuOption::handleRqs:
        break;
    
    case Core::Display::MainMenuOption::exit:
        
        break;
    default:
        break;
    }

}

bool Session::addSchool(School& newSchool)
{
    LOG((*logger_), "Adding new school: \"", newSchool.name_, "\"");
    if(wsMgr_->addSchool(newSchool))
    {
        sesData_->addSchool(newSchool);
        return true;
    }
    return false;
}

bool Session::removeSchool(School targetSchool)
{
    LOG((*logger_), "Removing existing school: \"", targetSchool.name_, "\"");
    if(wsMgr_->removeSchool(targetSchool))
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
    if(wsMgr_->addStudent(newStudent))
    {
        sesData_->addStudent(newStudent);
        return true;
    }
    return false;
}

bool Session::removeStudent(Student targetStudent)
{
    LOG((*logger_), "Removing student: id=", targetStudent.id_);
    if(wsMgr_->removeStudent(targetStudent))
    {
        sesData_->removeStudent(targetStudent);
        return true;
    }
    return false;
}

bool Session::addSubject(Subject& targetSubject)
{
    LOG((*logger_), "Adding new subject ", targetSubject.name_);
    if(wsMgr_->addSubject(targetSubject))
    {
        sesData_->addSubject(targetSubject);
        return true;
    }
    return false;
}

bool Session::removeSubject(Subject targetSubject)
{
    LOG((*logger_), "Attempting to remove subject ", targetSubject.name_);
    if(wsMgr_->removeSubject(targetSubject))
    {
        sesData_->removeSubject(targetSubject);
        return true;
    }
    return false;
}

bool Session::addGrade(Subject& targetSubject, Student& targetStudent, float value)
{
    LOG((*logger_), "Adding new grade for pair: ", targetSubject.name_, "-", targetStudent.email_, " value=", value);
    if(wsMgr_->addGrade(targetStudent, targetSubject, value))
    {
        sesData_->addGrade(targetSubject.id_, targetStudent.id_, value);
        return true;
    }
    return false;
}

bool Session::removeGrade(Subject targetSubject, Student targetStudent)
{
    LOG((*logger_), "Removing grade for pair: ", targetSubject.name_, "-", targetStudent.email_);
    if(wsMgr_->removeGrade(targetStudent, targetSubject))
    {
        sesData_->removeGrade(targetSubject.id_, targetSubject.id_);
        return true;
    }
    return false;
}

void Session::fetchGrades()
{
    LOG((*logger_), "Fetching grades");
    std::vector<std::vector<std::string>> dbGrades = wsMgr_->getGrades();
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