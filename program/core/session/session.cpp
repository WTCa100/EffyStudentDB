#include "session.hpp"

#include "../../types/entry.hpp"
#include "../../utilities/common/constants.hpp"
#include "../../utilities/common/stringManip.hpp"

#include <tuple>

using namespace Utilities::Common::Constants;

Session::Session(std::shared_ptr<WsManager> wsMgr):
    wsMgr_(wsMgr),
    logger_(wsMgr_->getLogger()),
    sAdapter_(std::make_shared<SqlAdapter>(logger_, wsMgr_->getSqlManager())),
    sesData_(std::make_shared<SessionData>()),
    display_(Menu(logger_, sesData_, sAdapter_))
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

void Session::run()
{
    LOG((*logger_), "Main run function called");
    bool exit = false;
    do {
        Core::Display::MainMenuOption op = display_.showMainMenu();
        switch (op)
        {
            case Core::Display::MainMenuOption::manageDb :
                {
                    fetchAll();
                    Action command;
                    do {
                        command = display_.manageDatabase();
                        handleAction(command);
                    } while (command.getCommand() != Core::ActionType::Short::actionExit);
                    dropAll();
                    break;
                }
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

bool Session::handleIndirectAction(const Action& userAction)
{
    LOG((*logger_), "Handling Indirect Action");
    std::string userCommand = userAction.getCommand();
    std::string userTarget  = userAction.getTarget();

    if (userTarget != g_tableCourseAttendees)
    {
        LOG((*logger_), "Failed to handle action! User target != ", g_tableCourseAttendees, " while command = ", userCommand,
            " Actual target = ", userTarget);
        std::cout << "An error occured while handling your action.\n";
        return false;
    }

    // For now the only indirect action we have would be assign and drop - they both have the same structure. command table
    // (always the same here) studentId courseId This will probably never change thus this implementation choise
    std::vector<std::string> userAdditionalValues = userAction.getAdditionalValues();
    uint16_t studentId, courseId;
    try
    {
        courseId = std::stoul(userAdditionalValues.at(0));
    }
    catch (const std::exception& e)
    {
        LOG((*logger_), "Failed to handle action! Exception thrown: ", e.what());
        std::cout << "An error occured while handling your action.\n";
        return false;
    }
    LOG((*logger_), "Extracted value: courseId=", courseId);

    std::shared_ptr<Course> targetCourse = std::static_pointer_cast<Course>(sesData_->getEntry(courseId, g_tableCourses));
    if (!targetCourse)
    {
        LOG((*logger_), "Failed to handle action! No course found in the database. Id=", courseId);
        std::cout << "Error, no such course with ID " << courseId << "\n";
        return false;
    }

    if (userCommand == Core::ActionType::Indirect::actionOpen)
    {
        if (sAdapter_->openCourse(courseId))
        {
            std::cout << "Successfully opened coruse " << targetCourse->name_ << " (" << courseId << ") \n";
            targetCourse->isOpen_ = Utilities::Common::Constants::OpenState::opened;
            return true;
        }
        else
        {
            LOG((*logger_), "Failed to open the course ", courseId, " at SQL level");
            std::cout << "An error occured while handling your action.\n";
            return false;
        }
    }

    if (userCommand == Core::ActionType::Indirect::actionClose)
    {
        if (sAdapter_->closeCourse(courseId))
        {
            std::cout << "Successfully closed coruse " << targetCourse->name_ << " (" << courseId << ") \n";
            targetCourse->isOpen_ = Utilities::Common::Constants::OpenState::closed;
            return true;
        }
        else
        {
            LOG((*logger_), "Failed to close the course ", courseId, " at SQL level");
            std::cout << "An error occured while handling your action.\n";
            return false;
        }
    }

    try
    {
        studentId = std::stoul(userAdditionalValues.at(1));
    }
    catch (const std::exception& e)
    {
        LOG((*logger_), "Failed to handle action! Exception thrown: ", e.what());
        std::cout << "An error occured while handling your action.\n";
        return false;
    }

    LOG((*logger_), "Extracted value: studentId=", studentId);

    std::shared_ptr<Student> targetStudent = std::static_pointer_cast<Student>(sesData_->getEntry(studentId, g_tableStudents));
    if (!targetStudent)
    {
        LOG((*logger_), "Failed to handle action! No student found in the database. Id=", studentId);
        std::cout << "Error, no such student with ID " << studentId << "\n";
        return false;
    }

    if (userCommand == Core::ActionType::Indirect::actionAssign)
    {
        if (sAdapter_->addAttendee(studentId, courseId))
        {
            LOG((*logger_), "Successfully added student ", studentId, " to course ", courseId);
            std::cout << "Successfully added student " << targetStudent->firstName_ << " " << targetStudent->lastName_
                      << " to the course \"" << targetCourse->name_ << "\"\n";
        }
        else { return false; }
    }
    else
    {
        if (sAdapter_->removeAttendee(studentId, courseId))
        {
            LOG((*logger_), "Successfully dropped student ", studentId, " from course ", courseId);
            std::cout << "Successfully dropped student " << targetStudent->firstName_ << " " << targetStudent->lastName_
                      << " from the course \"" << targetCourse->name_ << "\"\n";
        }
        else { return false; }
    }
    return true;
}

bool Session::handleDirectAction(const Action& userAction)
{
    LOG((*logger_), "Executing direct action");
    // Action is correct here - guaranteed
    std::string userCommand = userAction.getCommand();

    LOG((*logger_), "Action got command: ", userCommand);
    if (userCommand == Core::ActionType::Short::actionExit)
    {
        LOG((*logger_), "Normal exit");
        std::cout << "Exiting management mode.\n";
        return true;
    }

    std::string userTarget = userAction.getTarget();
    LOG((*logger_), "Action got target: ", userTarget);

    std::shared_ptr<Entry> concreteEntry = makeConcreteType(userTarget);

    // Select correct function
    if (userCommand == Core::ActionType::Direct::actionAdd)
    {
        LOG((*logger_), "Abstract add: ", userAction.getTarget());
        std::cout << "New entry:\n";
        concreteEntry.get()->userConstruct();
        if (sAdapter_->addEntry(*concreteEntry))
        {
            sesData_->addEntry(concreteEntry);
            std::cout << "Added new entry to table: " << userTarget << "\n";
            return true;
        }

        std::cout << "Failed to add given entry due to database error.\n";
        return false;
    }

    std::cout << "Matching values:\n";
    std::unordered_map<std::string, std::string> attrs = concreteEntry->userConstruct(false);
    std::string filter                                 = sAdapter_->makeFilter(attrs, Utilities::Sql::filterAnyMatch);

    LOG((*logger_), "Filter to lookup: ", filter);
    std::vector<std::shared_ptr<Entry>> affectedEntries = sAdapter_->getEntries(userTarget, filter);

    LOG((*logger_), "Link affected entries with data sessions")
    for (auto& e : affectedEntries) { *e = *e->mirrorMissing(sesData_->getEntry(e->id_, e->associatedTable_)); }

    if (affectedEntries.empty())
    {
        std::cout << "No match was found in the database!\n";
        return true;
    }

    // Nothing much has to be done while finding
    std::cout << "Selected entries:\n";
    display_.showSelection(affectedEntries);
    if (userCommand == Core::ActionType::Direct::actionFind) { return true; }

    if (userCommand == Core::ActionType::Direct::actionEdit || userCommand == Core::ActionType::Direct::actionRemove)
    {
        if (!display_.promptAlterAll(filter, affectedEntries.size()))
        {
            LOG((*logger_), "Aborting procedure");
            return true;
        }
    }

    // For each affected entry delete
    if (userCommand == Core::ActionType::Direct::actionRemove)
    {
        for (const auto& val : affectedEntries)
        {
            if (sAdapter_->removeEntry(*val)) { sesData_->removeEntry(val->id_, userTarget); }
        }
        std::cout << "Removed entry from table " << userTarget << "\n";
        return true;
    }

    // For each affected entry update
    if (userCommand == Core::ActionType::Direct::actionEdit)
    {
        // Get new values 1st
        std::cout << "Modifying existing values.\n";
        std::shared_ptr<Entry> entriesNew                            = makeConcreteType(userTarget);
        std::unordered_map<std::string, std::string> changesToCommit = entriesNew->userConstruct(false);
        for (auto& val : affectedEntries)
        {
            std::shared_ptr<Entry> changedEntries = makeConcreteType(userTarget);
            *changedEntries                       = *entriesNew->mirrorMissing(val);
            // Make a copy of a given filter entry
            if (sAdapter_->updateEntry(val->id_, changesToCommit, val->associatedTable_))
            {
                sesData_->updateEntry(val->id_, changedEntries);
            }
        }
        std::cout << "Updated entry in table " << userTarget << "\n";
        return true;
    }

    return false;
}

bool Session::handleAction(const Action& userAction)
{
    LOG((*logger_), "Handling action");
    if (Action::isCommandIndirect(userAction.getCommand())) { return handleIndirectAction(userAction); }
    else { return handleDirectAction(userAction); }
}

void Session::deleteCourseSubjectWeight(const std::shared_ptr<CourseSubjectWeight> targetWeight)
{
    LOG((*logger_), "Common CourseSubjectWeight deletion");
    std::shared_ptr<abstractTypeList> weightCourses = sesData_->getEntries(g_tableCourses);
    if (weightCourses->contains(targetWeight->courseId_)) { weightCourses->erase(targetWeight->courseId_); }
}

void Session::deleteGrade(std::shared_ptr<Grade> targetGrade)
{
    LOG((*logger_), "Common Grade deletion");
    std::shared_ptr<abstractTypeList> gradeStudent = sesData_->getEntries(g_tableStudents);
    if (gradeStudent->contains(targetGrade->studentId_)) { gradeStudent->erase(targetGrade->studentId_); }
}

std::shared_ptr<Entry> Session::makeConcreteType(const std::string& tableName) const
{
    LOG((*logger_), "Getting concrete type from target table: ", tableName);
    std::shared_ptr<Entry> tmp;
    // Extract concrete type
    if (tableName == g_tableSchools) { tmp = std::make_shared<School>(); }
    else if (tableName == g_tableStudents) { tmp = std::make_shared<Student>(); }
    else if (tableName == g_tableSubjects) { tmp = std::make_shared<Subject>(); }
    else if (tableName == g_tableCourses) { tmp = std::make_shared<Course>(); }
    else if (tableName == g_tableGrades) { tmp = std::make_shared<Grade>(); }
    else if (tableName == g_tableCourseSubjectWeight) { tmp = std::make_shared<CourseSubjectWeight>(); }
    else { tmp = std::make_shared<Srequest>(); }
    return tmp;
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

Session::~Session() { LOG((*logger_), "Session teardown!"); }