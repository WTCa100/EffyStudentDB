#include "session.hpp"

#include "../../types/entry.hpp"
#include "../../utilities/common/constants.hpp"
#include "../../utilities/common/stringManip.hpp"
#include "./commands/CommandFactory.hpp"

#include <tuple>

using namespace Utilities::Common::Constants;

Session::Session(std::shared_ptr<WsManager> wsMgr):
    wsMgr_(wsMgr),
    logger_(wsMgr_->getLogger()),
    sAdapter_(std::make_shared<SqlAdapter>(logger_, wsMgr_->getSqlManager())),
    sesData_(std::make_shared<SessionData>()),
    display_(Menu(logger_))
{
    LOG((*logger_), "Session established");
}

void Session::dropAll()
{
    LOG((*logger_), "Dropping locally allocated entries");
    sesData_->clearEntries();
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
    fetchCourseSubjectWeight();
    LOG((*logger_), "Fetching done");
}

void Session::fetchSchools()
{
    LOG((*logger_), "Fetching schools");
    std::vector<Core::Types::School> dbSchools = sAdapter_->getSchools();
    LOG((*logger_), "Got ", dbSchools.size(), " Entries");
    for (const auto& entry : dbSchools) { sesData_->addEntry(std::make_shared<School>(entry)); }
}

void Session::fetchStudents()
{
    LOG((*logger_), "Fetching students");
    std::vector<Core::Types::Student> dbStudents = sAdapter_->getStudents();
    LOG((*logger_), "Got ", dbStudents.size(), " Entries");
    for (const auto& entry : dbStudents)
    {
        std::shared_ptr<Student> concreteStudent = std::make_shared<Student>(entry);
        sesData_->addEntry(concreteStudent);

        const uint16_t schoolId              = entry.schoolId_;
        std::shared_ptr<School> targetSchool = std::dynamic_pointer_cast<School>(sesData_->getEntry(schoolId, g_tableSchools));
        if (targetSchool) { (*targetSchool).students_.insert(entry.id_); }
        else { std::runtime_error("Could not fetch school data with the following data: ID=" + std::to_string(schoolId)); }
    }
}

void Session::fetchSubjects()
{
    LOG((*logger_), "Fetching subjects");
    std::vector<Core::Types::Subject> dbSubjects = sAdapter_->getSubjects();
    LOG((*logger_), "Got ", dbSubjects.size(), " Entries");
    for (const auto& entry : dbSubjects) { sesData_->addEntry(std::make_shared<Subject>(entry)); }
}

void Session::fetchCourses()
{
    LOG((*logger_), "Fetching courses");
    std::vector<Core::Types::Course> dbCourses = sAdapter_->getCourses();
    for (const auto& entry : dbCourses) { sesData_->addEntry(std::make_shared<Course>(entry)); }
}

void Session::fetchSrequests()
{
    LOG((*logger_), "Fetching student requests");
    std::vector<Core::Types::Request::Srequest> dbSrequests = sAdapter_->getSrequests();
    for (const auto& entry : dbSrequests) { sesData_->addEntry(std::make_shared<Srequest>(entry)); }
}


void Session::fetchGrades()
{
    LOG((*logger_), "Fetching grades");

    std::shared_ptr<abstractTypeList> students = sesData_->getEntries(g_tableStudents);
    std::shared_ptr<abstractTypeList> subjects = sesData_->getEntries(g_tableSubjects);
    std::vector<Grade> grades                  = sAdapter_->getGrades();
    LOG((*logger_), "Got ", grades.size(), " entries from the database");
    for (Grade g : grades)
    {
        std::shared_ptr<Grade> currentGrade = std::make_shared<Grade>(g);
        if (students->contains(g.studentId_))
        {
            std::shared_ptr<Student> currentStudent = std::dynamic_pointer_cast<Student>(students->at(currentGrade->studentId_));
            currentGrade->studentName_              = currentStudent->firstName_ + " " + currentStudent->lastName_;
            currentStudent->grades_.insert(std::make_pair(currentGrade->id_, currentGrade));
        }
        else
        {
            LOG((*logger_), "Grade ", currentGrade->id_, " tried to associate with studnet: ", currentGrade->studentId_,
                " which does not exist.")
            currentGrade->studentName_ = "";
        }

        if (subjects->contains(g.subjectId_))
        {
            std::shared_ptr<Subject> currentSubject = std::dynamic_pointer_cast<Subject>(subjects->at(currentGrade->subjectId_));
            currentGrade->subjectName_              = currentSubject->name_;
        }
        else
        {
            LOG((*logger_), "Grade ", g.id_, " tried to associate with subject:", currentGrade->subjectId_,
                " which does not exist.")
            currentGrade->subjectName_ = "";
        }
        sesData_->addEntry(currentGrade);
    }
}

void Session::fetchCourseSubjectWeight()
{
    LOG((*logger_), "Fetching Course to subject weight");
    std::shared_ptr<abstractTypeList> courses  = sesData_->getEntries(g_tableCourses);
    std::shared_ptr<abstractTypeList> subjects = sesData_->getEntries(g_tableSubjects);
    std::vector<CourseSubjectWeight> weights   = sAdapter_->getCourseSubjectWeight();
    for (CourseSubjectWeight weight : weights)
    {
        std::shared_ptr<CourseSubjectWeight> currentWeight = std::make_shared<CourseSubjectWeight>(weight);
        if (courses->contains(currentWeight->courseId_))
        {
            std::shared_ptr<Course> currentCourse = std::dynamic_pointer_cast<Course>(courses->at(currentWeight->courseId_));
            currentWeight->courseName_            = currentCourse->name_;
            currentCourse->subjectWithWeight_.insert(std::make_pair(currentWeight->id_, currentWeight));
        }
        else
        {
            LOG((*logger_), "Weight ", currentWeight->id_, " tried to associate with course:", currentWeight->courseId_,
                " which does not exist.")
            currentWeight->courseName_ = "";
        }

        if (subjects->contains(currentWeight->subjectId_))
        {
            std::shared_ptr<Subject> currentSubject = std::dynamic_pointer_cast<Subject>(subjects->at(currentWeight->subjectId_));
            currentWeight->subjectName_             = currentSubject->name_;
        }
        else
        {
            LOG((*logger_), "Weight ", currentWeight->id_, " tried to associate with subject:", currentWeight->subjectId_,
                " which does not exist.")
            currentWeight->courseName_ = "";
        }
        sesData_->addEntry(currentWeight);
    }
}

void Session::manageDatabase()
{
    using Core::Commands::CommandFactory;
    using Core::Commands::ICommand;
    fetchAll();
    CommandFactory commandFactory = CommandFactory(logger_, sAdapter_, sesData_);
    do {
        LOG((*logger_), "Entered, database management");
        std::string rawCommand            = Utilities::InputHandler::toUpper(Utilities::InputHandler::getStringBeauty("Command"));
        std::unique_ptr<ICommand> command = commandFactory.makeCommand(rawCommand);

        if (!command)
        {
            LOG((*logger_), "Got invalid command: \"", rawCommand, "\"");
            std::cout << "Invalid command!\n";
            continue;
        }

        if (command->name() == "exit") { break; }

        if (command->exec())
        {
            LOG((*logger_), "Command: ", command->name(), " executed succesfully");
            std::cout << "Command executed successfully!\n";
        }
    } while (true);
    dropAll();
}

void Session::run()
{
    LOG((*logger_), "Main run function called");
    bool exit = false;
    do {
        Core::Display::MainMenuOption op = display_.showMainMenu();
        switch (op)
        {
            case Core::Display::MainMenuOption::manageDb : manageDatabase(); break;

            case Core::Display::MainMenuOption::handleRqs :
                {
                    RequestResolver requestCalculator = RequestResolver(logger_, sAdapter_, sesData_);
                    requestCalculator.run();
                    break;
                }
            case Core::Display::MainMenuOption::exit : exit = true; LOG((*logger_), "Exiting application");
            default : break;
        }

    } while (!exit);
}

Session::~Session() { LOG((*logger_), "Session teardown!"); }