#include "session.hpp"

#include "../../types/entry.hpp"
#include "../../utilities/common/constants.hpp"
#include "../../utilities/common/stringManip.hpp"

using namespace Utilities::Common::Constants;

Session::Session(std::shared_ptr<WsManager> wsMgr):
    wsMgr_(wsMgr),
    logger_(wsMgr_->getLogger()),
    sAdapter_(std::make_unique<SqlAdapter>(logger_, wsMgr_->getSqlManager())),
    sesData_(std::make_shared<SessionData>()),
    display_(std::make_unique<Menu>(logger_, sesData_))
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
    fetchAttendees();
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
        if (targetSchool) { (*targetSchool).students_.insert(std::make_pair(entry.id_, concreteStudent)); }
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

void Session::fetchAttendees()
{
    LOG((*logger_), "Fetch course attendees");
    std::vector<std::pair<uint16_t, uint16_t>> dbStudentCoursePair = sAdapter_->getAttendees();
    for (const auto& entry : dbStudentCoursePair)
    {
        uint16_t courseId, studentId;
        studentId = entry.first;
        courseId  = entry.second;
        std::shared_ptr<Student> targetStudent =
            std::static_pointer_cast<Student>(sesData_->getEntry(studentId, g_tableStudents));
        std::shared_ptr<Course> targetCourse = std::static_pointer_cast<Course>(sesData_->getEntry(courseId, g_tableCourses));
        targetStudent->attendingCourses_.insert(std::make_pair(courseId, targetCourse->name_));
        targetCourse->attendees_.insert(std::make_pair(studentId, targetStudent));
    }
}

void Session::run()
{
    LOG((*logger_), "Main run function called");
    bool exit = false;
    do {
        Core::Display::MainMenuOption op = display_->showMainMenu();
        switch (op)
        {
            case Core::Display::MainMenuOption::manageDb :
                {
                    fetchAll();
                    Action command;
                    do {
                        command = display_->manageDatabase();
                        handleAction(command);
                    } while (command.getCommand() != "EXIT");
                    dropAll();
                    break;
                }
            case Core::Display::MainMenuOption::handleRqs : break;
            case Core::Display::MainMenuOption::exit : exit = true; LOG((*logger_), "Exiting application");
            default : break;
        }

    } while (!exit);
}

bool Session::handleIndirectAction(const Action& userAction)
{
    LOG((*logger_), "Handling Indirect Action");

    std::cout << "DBG: " << userAction.getCommand() << " :=> target " << userAction.getTarget() << "\n";
    return true;
}

bool Session::handleDirectAction(const Action& userAction)
{
    LOG((*logger_), "Executing direct action");
    // Action is correct here - guaranteed
    std::string userCommand = userAction.getCommand();

    LOG((*logger_), "Action got command: ", userCommand);
    if (userCommand == "EXIT")
    {
        LOG((*logger_), "Normal exit");
        std::cout << "Exiting management mode.\n";
        return true;
    }

    std::string userTarget = userAction.getTarget();
    LOG((*logger_), "Action got target: ", userTarget);

    std::shared_ptr<Entry> concreteEntry = makeConcreteType(userTarget);

    // Select correct function
    if (userCommand == "ADD")
    {
        LOG((*logger_), "Abstract add: ", userAction.getTarget());
        std::cout << "New entry:\n";
        concreteEntry.get()->userConstruct();
        // @TODO check if entry is valid
        if (sAdapter_->addEntry(*concreteEntry))
        {
            sesData_->addEntry(concreteEntry);
            onAdd(concreteEntry);
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
    for (auto e : affectedEntries) { *e = *e->mirrorMissing(sesData_->getEntry(e->id_, e->associatedTable_)); }

    if (affectedEntries.empty())
    {
        std::cout << "No match was found in the database!\n";
        return true;
    }

    // Nothing much has to be done while finding
    std::cout << "Selected entries:\n";
    display_->showSelection(affectedEntries);
    if (userCommand == "FIND") { return true; }

    if (userCommand == "REMOVE" || userCommand == "UPDATE")
    {
        if (!display_->promptAlterAll(filter, affectedEntries.size()))
        {
            LOG((*logger_), "Aborting procedure");
            return true;
        }
    }

    // For each affected entry delete
    if (userCommand == "REMOVE")
    {
        for (const auto& val : affectedEntries)
        {
            if (sAdapter_->removeEntry(*val))
            {
                onDelete(val);
                sesData_->removeEntry(val->id_, userTarget);
            }
        }
        return true;
    }

    // For each affected entry update
    if (userCommand == "UPDATE")
    {
        // Get new values 1st
        std::cout << "Modifying existing values.\n";
        std::shared_ptr<Entry> entriesNew = makeConcreteType(userTarget);
        entriesNew->userConstruct(false);
        for (auto& val : affectedEntries)
        {
            std::shared_ptr<Entry> changedEntries = makeConcreteType(userTarget);
            *changedEntries                       = *entriesNew->mirrorMissing(val);
            // Make a copy of a given filter entry
            if (sAdapter_->updateEntry(*val, *changedEntries))
            {
                onUpdate(val, changedEntries);
                sesData_->updateEntry(val->id_, changedEntries);
            }
        }
    }

    return false;
}

bool Session::handleAction(const Action& userAction)
{
    LOG((*logger_), "Handling action");
    if (Action::isCommandIndirect(userAction.getCommand())) { return handleIndirectAction(userAction); }
    else { return handleDirectAction(userAction); }
}

void Session::onAdd(const std::shared_ptr<Entry> newEntry)
{
    const std::string targetTable = newEntry->associatedTable_;
    LOG((*logger_), "Called onAdd linking cleanup - target table: ", targetTable);
    // School, Subject, StudentRequest and Course does not need a special handle
    if (targetTable == g_tableCourses || targetTable == g_tableSubjects || targetTable == g_tableSchools ||
        targetTable == g_tableStudentRequest)
    {
        LOG((*logger_), "Simple type - no clean needed");
        return;
    }

    // If a new student is present add it to the school list and sRequests
    if (targetTable == g_tableStudents)
    {
        std::shared_ptr<Student> concreteStudent = std::dynamic_pointer_cast<Student>(newEntry);
        LOG((*logger_), "Student linking - id: ", concreteStudent->id_, " schoolId: ", concreteStudent->schoolId_);

        // Only add to the target school
        std::shared_ptr<School> studentSchool =
            std::dynamic_pointer_cast<School>(sesData_->getEntry(concreteStudent->schoolId_, g_tableSchools));
        if (!studentSchool)
        {
            LOG((*logger_), "Student added to a non existing school! This may lead to undefined behavior. Student signature: ",
                concreteStudent, " id: ", concreteStudent->id_);
            return;
        }
        studentSchool->students_.insert(std::make_pair(newEntry->id_, concreteStudent));
        // std::dynamic_pointer_cast<School>(sesData_->getEntry(concreteStudent->schoolId_, g_tableSchools))
        // 	->students_.insert(std::make_pair(newEntry->id_, concreteStudent));
    }

    if (targetTable == g_tableGrades)
    {
        std::shared_ptr<Grade> concreteGrade = std::dynamic_pointer_cast<Grade>(newEntry);
        LOG((*logger_), "Grade added - linking with student: ", concreteGrade->studentId_,
            " subject: ", concreteGrade->subjectId_);

        std::shared_ptr<Student> refStudent =
            std::dynamic_pointer_cast<Student>(sesData_->getEntry(concreteGrade->studentId_, g_tableStudents));
        if (!refStudent)
        {
            LOG((*logger_),
                "Grade added to a non existing student! This may lead to undefined behavior. Grade signature: ", concreteGrade,
                " id: ", concreteGrade->id_);
            return;
        }

        std::shared_ptr<Subject> refSubject =
            std::dynamic_pointer_cast<Subject>(sesData_->getEntry(concreteGrade->subjectId_, g_tableSubjects));
        if (!refSubject)
        {
            LOG((*logger_),
                "Grade added to a non existing subject! This may lead to undefined behavior. Grade signature: ", concreteGrade,
                " id: ", concreteGrade->id_);
            return;
        }
        concreteGrade->studentName_ = refStudent->firstName_ + " " + refStudent->lastName_;
        concreteGrade->subjectName_ = refSubject->name_;
        refStudent->grades_.insert(std::make_pair(concreteGrade->id_, concreteGrade));
    }

    if (targetTable == g_tableCourseSubjectWeight)
    {
        std::shared_ptr<CourseSubjectWeight> concreteWeight = std::dynamic_pointer_cast<CourseSubjectWeight>(newEntry);
        LOG((*logger_), "Weight added - linking with course: ", concreteWeight->courseId_,
            " subject: ", concreteWeight->subjectId_);

        std::shared_ptr<Course> refCourse =
            std::dynamic_pointer_cast<Course>(sesData_->getEntry(concreteWeight->courseId_, g_tableCourses));
        if (!refCourse)
        {
            LOG((*logger_),
                "Weight added to a non existing course! This may lead to undefined behavior. Weight signature: ", concreteWeight,
                " id: ", concreteWeight->id_);
            return;
        }

        std::shared_ptr<Subject> refSubject =
            std::dynamic_pointer_cast<Subject>(sesData_->getEntry(concreteWeight->subjectId_, g_tableSubjects));
        if (!refSubject)
        {
            LOG((*logger_),
                "Grade added to a non existing subject! This may lead to undefined behavior. Grade signature: ", concreteWeight,
                " id: ", concreteWeight->id_);
            return;
        }
        concreteWeight->courseName_  = refCourse->name_;
        concreteWeight->subjectName_ = refSubject->name_;
        refCourse->subjectWithWeight_.insert(std::make_pair(concreteWeight->id_, concreteWeight));
    }
}

void Session::onDelete(const std::shared_ptr<Entry> targetEntry)
{
    const uint16_t id             = targetEntry->id_;
    const std::string targetTable = targetEntry->associatedTable_;
    LOG((*logger_), "Called onDelete linking cleanup - target table: ", targetTable);

    // There are little to no safe types and each have to be handled.
    // If a school is deleted - inform about leaving students without school
    if (targetTable == g_tableSchools)
    {
        std::shared_ptr<School> concreteSchool =
            std::dynamic_pointer_cast<School>(sesData_->getEntry(targetEntry->id_, targetTable));
        LOG((*logger_), "Deleting school id: ", id, " name: ", concreteSchool->name_);
        if (!concreteSchool->students_.empty())
        {
            // @todo Add key constrain so this won't be an issue
            std::cout << "Deleting " << concreteSchool << " left " << concreteSchool->students_.size() << " without any school\n";
        }
        return;
    }

    // If a student is deleted - grades are erased automatically, schools has to remove one entry from list and associated student
    // request deleted.
    if (targetTable == g_tableStudents)
    {
        std::shared_ptr<Student> concreteStudent =
            std::dynamic_pointer_cast<Student>(sesData_->getEntry(targetEntry->id_, targetTable));
        LOG((*logger_), "Deleting student id: ", id);
        // Remove student from a given school
        std::shared_ptr<School> studentSchool =
            std::dynamic_pointer_cast<School>(sesData_->getEntry(concreteStudent->schoolId_, g_tableSchools));
        if (studentSchool)
        {
            LOG((*logger_), "Removing student id = ", id, " from associated school: ", concreteStudent->schoolId_);
            studentSchool->students_.erase(id);
        }

        // Remove students' requests
        std::shared_ptr<abstractTypeList> requestLists = sesData_->getEntries(g_tableStudentRequest);
        if (!requestLists)
        {
            LOG((*logger_), "No student reuqests table found to delete given student");
            return;
        }

        for (auto& request : *requestLists)
        {
            std::shared_ptr<Srequest> req = std::dynamic_pointer_cast<Srequest>(request.second);
            if (req->studentId_ == id) { requestLists->erase(req->id_); }
        }

        // @TODO - for some reason when deleting student - grades are not automatically deleted
        LOG((*logger_), "Deleting linked grades - count = ", concreteStudent->grades_.size());
        for (auto& g : concreteStudent->grades_)
        {
            std::cout << "DBG: deleting: " << g.second->toString() << "\n";
            sesData_->removeEntry(g.first, g_tableGrades);
        }
        concreteStudent->grades_.clear();

        // LOG((*logger_), "Linking cleaned with deleted student\n");
        return;
    }

    // If a given subject is deleted we only need to handle the course to subject weight - and eventually student grade with given
    // id
    if (targetTable == g_tableSubjects)
    {
        std::shared_ptr<Subject> targetSubject = std::dynamic_pointer_cast<Subject>(targetEntry);
        LOG((*logger_), "Deleting subject: ", targetSubject->id_);
        // Remove subject from weights
        std::shared_ptr<abstractTypeList> refWeights = sesData_->getEntries(g_tableCourseSubjectWeight);
        LOG((*logger_), "Handling linked course weights len=", refWeights->size())
        for (auto& weight : *refWeights)
        {
            std::shared_ptr<CourseSubjectWeight> concreteWeight = std::dynamic_pointer_cast<CourseSubjectWeight>(weight.second);
            if (concreteWeight->subjectId_ == id)
            {
                LOG((*logger_), "Associated weight: ", concreteWeight->id_);
                deleteCourseSubjectWeight(concreteWeight);
            }
        }
        // Remove subjects from grades
        std::shared_ptr<abstractTypeList> refGrades = sesData_->getEntries(g_tableGrades);
        LOG((*logger_), "Handling linked grades len=", refGrades->size());
        for (auto& grade : *refGrades)
        {
            std::shared_ptr<Grade> concreteGrade = std::dynamic_pointer_cast<Grade>(grade.second);
            if (concreteGrade->subjectId_ == id)
            {
                LOG((*logger_), "Associated grade", concreteGrade->id_);
                deleteGrade(concreteGrade);
            }
        }
    }

    // If a given course is deleted a student request with that course id will be declined, the subject weight will be erased
    // automatically
    if (targetTable == g_tableStudentRequest)
    {
        std::shared_ptr<Srequest> refSRequest = std::dynamic_pointer_cast<Srequest>(targetEntry);
        LOG((*logger_), "Deleting srequest: ", targetTable);
    }

    // If a given grade is deleted only delete it from target student - no handle yet
    if (targetTable == g_tableGrades)
    {
        LOG((*logger_), "Plain grade delete: ", targetEntry->id_);
        deleteGrade(std::dynamic_pointer_cast<Grade>(targetEntry));
    }

    // If a course weight is deleted only delete it from target course - no handle yet
    if (targetTable == g_tableCourseSubjectWeight)
    {
        LOG((*logger_), "Plain weight delete: ", targetEntry->id_);
        deleteCourseSubjectWeight(std::dynamic_pointer_cast<CourseSubjectWeight>(targetEntry));
    }
}

void Session::onUpdate(std::shared_ptr<Entry> oldEntry, const std::shared_ptr<Entry> newEntry)
{
    const uint16_t id             = oldEntry->id_;
    newEntry->id_                 = id;
    const std::string targetTable = oldEntry->associatedTable_;
    LOG((*logger_), "Editing entry id:", id, " targetTable:", targetTable);

    // If a student is changed - Assign to new school if neede
    if (targetTable == g_tableStudents)
    {
        std::shared_ptr<Student> concreteStudentOld = std::dynamic_pointer_cast<Student>(oldEntry);
        std::shared_ptr<Student> concreteStudentNew = std::dynamic_pointer_cast<Student>(newEntry);
        uint16_t schoolIdOld                        = concreteStudentOld->schoolId_;
        uint16_t schoolIdNew                        = concreteStudentNew->schoolId_;
        if (schoolIdNew != schoolIdOld)
        {
            LOG((*logger_), "Entry has a different link. Id= ", id);
            std::shared_ptr<School> schoolOld =
                std::dynamic_pointer_cast<School>(sesData_->getEntry(schoolIdOld, g_tableSchools));
            std::shared_ptr<School> schoolNew =
                std::dynamic_pointer_cast<School>(sesData_->getEntry(schoolIdNew, g_tableSchools));
            schoolOld->students_.erase(id);
            schoolNew->students_.insert(std::make_pair(id, concreteStudentOld));
        }
    }

    // If a grade is edited - reassign to different student or subject if needed
    if (targetTable == g_tableGrades)
    {
        std::shared_ptr<Grade> concreteGradeOld = std::dynamic_pointer_cast<Grade>(oldEntry);
        std::shared_ptr<Grade> concreteGradeNew = std::dynamic_pointer_cast<Grade>(newEntry);
        uint16_t studentIdOld                   = concreteGradeOld->studentId_;
        uint16_t studentIdNew                   = concreteGradeNew->studentId_;
        if (studentIdOld != studentIdNew)
        {
            LOG((*logger_), "Entry has a different link. Id= ", id);
            std::shared_ptr<Student> studentOld =
                std::dynamic_pointer_cast<Student>(sesData_->getEntry(studentIdOld, g_tableStudents));
            std::shared_ptr<Student> studentNew =
                std::dynamic_pointer_cast<Student>(sesData_->getEntry(studentIdNew, g_tableStudents));
            studentOld->grades_.erase(id);
            studentNew->grades_.insert(std::make_pair(id, concreteGradeOld));
            concreteGradeNew->studentName_;
        }

        uint16_t subjectIdOld = concreteGradeOld->subjectId_;
        uint16_t subjectIdNew = concreteGradeNew->subjectId_;
        if (subjectIdOld != subjectIdNew)
        {
            std::shared_ptr<Subject> subjectNew =
                std::dynamic_pointer_cast<Subject>(sesData_->getEntry(subjectIdNew, g_tableSubjects));
            concreteGradeNew->subjectName_ = subjectNew->name_;
        }
    }

    // If a CourseSubjectWeight is changed - reasign to different course
    if (targetTable == g_tableCourseSubjectWeight)
    {
        std::shared_ptr<CourseSubjectWeight> concreteWeightOld = std::dynamic_pointer_cast<CourseSubjectWeight>(oldEntry);
        std::shared_ptr<CourseSubjectWeight> concreteWeightNew = std::dynamic_pointer_cast<CourseSubjectWeight>(newEntry);
        uint16_t courseIdOld                                   = concreteWeightOld->courseId_;
        uint16_t courseIdNew                                   = concreteWeightNew->courseId_;
        if (courseIdOld != courseIdNew)
        {
            LOG((*logger_), "Entry has a different link. Id= ", id);
            std::shared_ptr<Course> courseOld =
                std::dynamic_pointer_cast<Course>(sesData_->getEntry(courseIdOld, g_tableCourses));
            std::shared_ptr<Course> courseNew =
                std::dynamic_pointer_cast<Course>(sesData_->getEntry(courseIdNew, g_tableCourses));
            courseOld->subjectWithWeight_.erase(id);
            courseNew->subjectWithWeight_.insert(std::make_pair(id, concreteWeightOld));
            courseNew->name_ = concreteWeightNew->courseName_;
        }

        uint16_t subjectIdOld = concreteWeightOld->subjectId_;
        uint16_t subjectIdNew = concreteWeightNew->subjectId_;
        if (subjectIdOld != subjectIdNew)
        {
            std::shared_ptr<Subject> subjectNew =
                std::dynamic_pointer_cast<Subject>(sesData_->getEntry(subjectIdNew, g_tableSubjects));
            concreteWeightNew->subjectName_ = subjectNew->name_;
        }
    }
}

// Todo turn deletes into lambdas inside func where there are used
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
        if (courses->contains(currentWeight->id_))
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