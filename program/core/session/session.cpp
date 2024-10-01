#include "session.hpp"

Session::Session(std::shared_ptr<WsManager> wsMgr, std::shared_ptr<Logger> logger) : wsMgr_(wsMgr), logger_(logger), sesData_(std::make_unique<SessionData>())
{
    LOG((*logger_), "New session instanace created!");    
    LOG((*logger_), "Loading initial database entries");
    fetchAll();
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
    for(const auto& entry : dbSchools)
    {
        sesData_->addSchool(entry);
    }
}

void Session::fetchStudents()
{
    LOG((*logger_), "Fetching students");
    std::vector<Core::Types::Student> dbStudents = wsMgr_->getStudents();
    for(const auto& entry : dbStudents)
    {
        sesData_->addStudent(entry);
    }
}

void Session::fetchSubjects()
{
    LOG((*logger_), "Fetching subjects");
    std::vector<Core::Types::Subject> dbSubjects = wsMgr_->getSubjects();
    for(const auto& entry : dbSubjects)
    {
        std::cout << "At " << entry.name_ << "\n";
        sesData_->addSubject(entry);
    }
}

void Session::fetchGrades()
{
    LOG((*logger_), "Fetching grades");
    std::vector<std::vector<std::string>> dbGrades = wsMgr_->getGrades();
    for(auto& entry : dbGrades)
    {
        // (0) GRADE | (1) SUBID | (2) STUDENTID | (3) ID
        float currentGrade = std::stof(entry.at(0));
        uint16_t targetedStudent = std::stoul(entry.at(1));
        uint16_t targetedSubject = std::stoul(entry.at(2));
        sesData_->addGrade(targetedSubject, targetedStudent, currentGrade);
    }
}

Session::~Session()
{
    LOG((*logger_), "Session teardown!");
}