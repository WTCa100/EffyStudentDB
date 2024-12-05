#include"sqlAdapter.hpp"

#include "../../common/stringManip.hpp"

using Utilities::Sql::Types::Table;

namespace Utilities::Sql
{
    SqlAdapter::SqlAdapter(std::shared_ptr<Logger> logger, std::shared_ptr<SqlManager> sManager) : logger_(logger), sManager_(sManager) { LOG((*logger_), "Creation of SqlAdapter"); }

    std::vector<Core::Types::School> SqlAdapter::getSchools()
    {
        std::vector<Core::Types::School> schools;
        LOG((*logger_), "Fetching schools from the SQL DB");
        std::vector<std::string> rawEntries = sManager_->getEntriesFromTable("Schools");
        if(rawEntries.empty())
        {
            LOG((*logger_), "Called schools, but got no entries!");
            std::cout << "No schools! Either fail or sql has no schools\n";
            return {};
        }

        LOG((*logger_), "Got n=", rawEntries.size(), " school entries");
        for(auto e : rawEntries)
        {
            std::vector<std::string> tokenizedSchool = Utilities::Common::tokenize(e, '|');
            // Tokens are:
            // (0)NAME | (1)ID
            schools.push_back(Core::Types::School{static_cast<uint16_t>(std::stoul(tokenizedSchool.at(1))), tokenizedSchool.at(0), {}});
        }
        LOG((*logger_), "Schools tokenized and pushed into the list. Final vector size = ", schools.size(), " Raw entries size = ", rawEntries.size());
        return schools;
    }

    std::vector<Core::Types::Student> SqlAdapter::getStudents()
    {
        std::vector<Core::Types::Student> students;
        LOG((*logger_), "Fetching students from the SQL DB");
        // @TODO: Idea - instead of simple SELECT call - why not use 2nd param here to specify each attribute in the same order as the struct
        // This will make the code more readable and understandable.
        std::vector<std::string> rawEntries = sManager_->getEntriesFromTable("Students");
        if(rawEntries.empty())
        {
            LOG((*logger_), "Called students, but got no entries!");
            std::cout << "No students! Either fail or sql has no students\n";
            return {};
        }

        LOG((*logger_), "Got n=", rawEntries.size(), " student entries");
        for(auto e : rawEntries)
        {
            std::vector<std::string> tokenizedStudent = Utilities::Common::tokenize(e, '|');
            // Tokens are:
            // (0) EMAIL | (1)LASTNAME | (2) SECONDNAME | (3) FIRSTNAME | (4) SCHOOLID | (5) ID
            // Translates to:
            // (0) ID | (1) FIRSTNAME | (2) SECONDNAME | (3) LASTNAME | (4) EMAIL | (5) SCHOOLID 
            std::optional<std::string> secondName = std::nullopt;
            if(tokenizedStudent.at(2) != "NULL") { secondName = tokenizedStudent.at(1); }
            
            Core::Types::Student tmpStudent;
            tmpStudent.id_         = std::stoul(tokenizedStudent.at(5));
            tmpStudent.firstName_  = tokenizedStudent.at(3);
            tmpStudent.secondName_ = secondName;
            tmpStudent.lastName_   = tokenizedStudent.at(1);
            tmpStudent.email_      = tokenizedStudent.at(0);
            tmpStudent.grades_     = {};
            tmpStudent.schoolId_   = std::stoul(tokenizedStudent.at(4));
            students.push_back(tmpStudent);
        }
        LOG((*logger_), "Students tokenized and pushed into the list. Final vector size = ", students.size(), " Raw entries size = ", rawEntries.size());
        return students;
    }
    
    std::vector<Core::Types::Subject> SqlAdapter::getSubjects()
    {
        std::vector<Core::Types::Subject> subjects;
        LOG((*logger_), "Fetching subjects from the SQL DB");
        std::vector<std::string> rawEntries = sManager_->getEntriesFromTable("Subjects");
        if(rawEntries.empty())
        {
            LOG((*logger_), "Called subjects, but got no entries!");
            std::cout << "No subjects! Either fail or sql has no subjects\n";
            return {};
        }

        LOG((*logger_), "Got n=", rawEntries.size(), " subjects entries");
        for(auto e : rawEntries)
        {
            std::vector<std::string> tokenizedSubjects = Utilities::Common::tokenize(e, '|');
            // Tokens are:
            // (0)NAME | (1)ID
            subjects.push_back(Core::Types::Subject{static_cast<uint16_t>(std::stoul(tokenizedSubjects.at(1))), tokenizedSubjects.at(0)});
        }
        LOG((*logger_), "Subjects tokenized and pushed into the list. Final vector size = ", subjects.size(), " Raw entries size = ", rawEntries.size());
        return subjects;
    }

    std::vector<std::vector<std::string>> SqlAdapter::getGrades()
    {
        LOG((*logger_), "Fetching grades from the SQL DB");
        std::vector<std::string> rawEntries = sManager_->getEntriesFromTable("grades");
        if(rawEntries.empty())
        {
            LOG((*logger_), "Called grades, but got no entries!");
            std::cout << "No grades! Either fail or sql has no grades\n";
            return {};
        }

        std::vector<std::vector<std::string>> listOfGrades;
        LOG((*logger_), "Got n=", rawEntries.size(), " grades entries");
        for(auto e : rawEntries)
        {
            listOfGrades.push_back(Utilities::Common::tokenize(e, '|'));
        }
        LOG((*logger_), "Grades tokenized and pushed into the list. Final vector size = ", listOfGrades.size(), " Raw entries size = ", rawEntries.size());
        return listOfGrades;
    }

    std::vector<Core::Types::Course> SqlAdapter::getCourses()
    {
        std::vector<Core::Types::Course> courses;
        LOG((*logger_), "Fetching courses from the SQL DB");
        std::vector<std::string> rawEntries = sManager_->getEntriesFromTable("courses");
        if(rawEntries.empty())
        {
            LOG((*logger_), "Called courses, but got no entries!");
            std::cout << "No courses! Either fail or sql has no courses\n";
            return {};
        }
        
        LOG((*logger_), "Got n=", rawEntries.size(), " entries");
        for(auto e : rawEntries)
        {
            std::vector<std::string> tokenizedCourse = Utilities::Common::tokenize(e, '|');
            // Tokens are:
            // (0) baseMinimalPoints | (1) name | (2) maxStudentCount | (3) minStudentCount | (4) id
            // Translates to:
            // (0) id | (1) minStudentCount | (2) maxStudentsCount | (3) baseMinimalPoints | (4) avgStudentPoints | (5) name | (6) subjectWithWeight
            Core::Types::Course tmpCourse;
            tmpCourse.id_                   = std::stoul(tokenizedCourse.at(4));
            tmpCourse.minStudents_          = std::stoi(tokenizedCourse.at(3));
            tmpCourse.maxStudents_          = std::stoi(tokenizedCourse.at(2));
            tmpCourse.baseMinimalPoints_    = std::stoi(tokenizedCourse.at(0));
            tmpCourse.averageStudentPoints_ = 0.0f;
            tmpCourse.name_                 = tokenizedCourse.at(1);
            tmpCourse.subjectWithWeight_    = {};

            // For each course get the aproporiate
            mapSubjectToCourseWeight(tmpCourse);
            courses.push_back(std::move(tmpCourse)); // It's quite big move makes more sense here
        }
        LOG((*logger_), "Courses tokenized and pushed into the list. Final vector size = ", courses.size(), " Raw entries size = ", rawEntries.size());
        return courses;
    }

    std::vector<Core::Types::Request::Srequest> SqlAdapter::getSrequests()
    {
        std::vector<Core::Types::Request::Srequest> sRequests;
        LOG((*logger_), "Fetching student requests from the SQL DB");
        std::vector<std::string> rawEntries = sManager_->getEntriesFromTable("StudentRequest");
        
        if(rawEntries.empty())
        {
            LOG((*logger_), "Called courses, but got no entries!");
            std::cout << "No courses! Either fail or sql has no courses\n";
            return {};
        }
        
        LOG((*logger_), "Got n=", rawEntries.size(), " entries");
        for(auto e : rawEntries)
        {
            std::vector<std::string> tokenizedEntry = Utilities::Common::tokenize(e, '|');
            // Tokens are
            // (0) requestStatus | (1) courseId | (2) studentId | (3) RequestId |
            // Translates to
            // (0) RequestId | (1) studentId | (2) studentId | (3) requestStatus
            Request::Srequest newEntry(static_cast<uint32_t>(std::stoul(tokenizedEntry.at(3))),
                                       static_cast<uint16_t>(std::stoul(tokenizedEntry.at(2))),
                                       static_cast<uint16_t>(std::stoul(tokenizedEntry.at(1))),
                                       static_cast<Request::requestStatus>(std::stoul(tokenizedEntry.at(0))));
            sRequests.push_back(newEntry);
        }

        LOG((*logger_), "Courses tokenized and pushed into the list. Final vector size = ", sRequests.size(), " Raw entries size = ", rawEntries.size());
        return sRequests;
    }

    void SqlAdapter::mapSubjectToCourseWeight(Core::Types::Course& targetCourse)
    {
        LOG((*logger_), "Fetching subject to course weight");
        std::vector<std::string> rawEntries = sManager_->getEntriesFromTable("CourseSubjectWeigt", {"subjectId", "weight"}, ("courseId =" + targetCourse.id_));
        if(rawEntries.empty())
        {
            LOG((*logger_), "Course ", targetCourse.name_, " has no weights assigned to it.");
            return;
        }
        LOG((*logger_), "Mapping subjects to courses. Got n=", rawEntries.size(), " entries");
        for(auto e : rawEntries)
        {
            std::vector<std::string> tokenizedEntry = Utilities::Common::tokenize(e, '|');
            targetCourse.subjectWithWeight_.push_back(std::make_pair( std::stoul(tokenizedEntry.at(0)), std::stod(tokenizedEntry.at(1))));
        }
        LOG((*logger_), "Courses weight have been updated. New mapped subjects size = ", targetCourse.subjectWithWeight_.size());
        return;
    }

    uint16_t SqlAdapter::getLatestIdFromTable(std::string tblName)
    {
        LOG((*logger_), "Getting latest id from ", tblName);
        std::vector<std::string> result = sManager_->getEntriesFromTable(tblName, {"MAX(id)"});
        // Check if it's empty - such scenario would probably never occur?
        LOG((*logger_), "Max ID from table = ", std::stoul(result.at(0)));
        return static_cast<uint16_t>(std::stoul(result.at(0)));
    }

    bool SqlAdapter::addSchool(Core::Types::School& newSchool)
    {
        LOG((*logger_), "Adding new school \"", newSchool.name_, "\"");
        // Check if given entry exists
        // if(sManager_->) Check if entry already exists
        // Prepare atributes
        Table targetTable = sManager_->getTableSchema("Schools");
        if(sManager_->addEntryToTable(targetTable.getName(),
            { std::make_pair(targetTable.getAttributeByName("name") , newSchool.name_)}))
        {
            newSchool.id_ = getLatestIdFromTable(targetTable.getName());
            LOG((*logger_), "New school added. Name=", newSchool.name_, " id=", newSchool.id_);
            return true;
        }
    
        LOG((*logger_), "Could not add given school into the database. Name=", newSchool.name_);
        return false;
    }

    bool SqlAdapter::updateSchool(const School& targetSchool, const School& newSchool)
    {
        LOG((*logger_), "Updating target school ", targetSchool.name_, ". Compare has started");
        AttrsValues newValPacket{};
        // First compare 
        if(targetSchool.name_ != newSchool.name_)
        {
            LOG((*logger_), "Name mismatch ", targetSchool.name_, " <-> ", newSchool.name_);
            // For altering existing attribute there is no need to add any extra flags
            newValPacket.push_back(std::make_pair(Utilities::Sql::Types::Attribute("name"), newSchool.name_));
        }
        
        if(newValPacket.empty())
        {
            LOG((*logger_), "New School = Old School nothing needs to be altered");
            return true;
        }

        LOG((*logger_), "Changes were found, DB will be contacted");
        std::string condition = "id = " + std::to_string(targetSchool.id_);
        return sManager_->updateEntryFromTable("Schools", newValPacket, condition);
    }

    bool SqlAdapter::removeSchool(const Core::Types::School& removeSchool)
    {
        LOG((*logger_), "Attempting to remove school \"", removeSchool.name_, "\"");
        // TODO Check if exists - if not, nothing to delete and return true
        sManager_->removeEntryFromTable("Schools", removeSchool.id_);\
        // TODO verify
        return true;
    }

    bool SqlAdapter::addStudent(Core::Types::Student& newStudent)
    {
        LOG((*logger_), "Adding new student \"", newStudent.email_, "\"");
        // Check if given entry exists
        // if(sManager_->) Check if entry already exists
        // Prepare atributes
        Table targetTable = sManager_->getTableSchema("Students");
        if(sManager_->addEntryToTable(targetTable.getName(),
            { std::make_pair(targetTable.getAttributeByName("firstName") , newStudent.firstName_),
              std::make_pair(targetTable.getAttributeByName("secondName"), (newStudent.secondName_ ? newStudent.secondName_.value() : "" )),
              std::make_pair(targetTable.getAttributeByName("lastName"), newStudent.lastName_),
              std::make_pair(targetTable.getAttributeByName("email"), newStudent.email_),
              std::make_pair(targetTable.getAttributeByName("schoolId"), std::to_string(newStudent.schoolId_))}))
        {
            newStudent.id_ = getLatestIdFromTable(targetTable.getName());
            LOG((*logger_), "New student added. Email=", newStudent.email_, " id=", newStudent.id_);
            return true;
        }
        LOG((*logger_), "Could not add given student into the database. Name=", newStudent.firstName_, " ", newStudent.lastName_, " email=", newStudent.email_);
        return false;
    }

    bool SqlAdapter::removeStudent(const Core::Types::Student& targetStudent)
    {
        LOG((*logger_), "Attempting to remove student \"", targetStudent.email_, "\"");
        sManager_->removeEntryFromTable("Students", targetStudent.id_);
        return true;
    }

    bool SqlAdapter::addSubject(Core::Types::Subject& newSubject)
    {
        LOG((*logger_), "Adding new subject \"", newSubject.name_, "\"");
        Table targetTable = sManager_->getTableSchema("Subjects");
        if(sManager_->addEntryToTable(targetTable.getName(), 
            {std::make_pair(targetTable.getAttributeByName("name"), newSubject.name_)}))
        {
            newSubject.id_ = getLatestIdFromTable(targetTable.getName());
            LOG((*logger_), "New subject added. Name=", newSubject.name_, " id=", newSubject.id_);
            return true;
        }
        LOG((*logger_), "Could not add given subject into the database. Name=", newSubject.name_);
        return false;
    }

    bool SqlAdapter::removeSubject(const Core::Types::Subject& targetSubject)
    {
        LOG((*logger_), "Attempting to remove subject \"", targetSubject.name_, "\"");
        sManager_->removeEntryFromTable("Subjects", targetSubject.id_);
        return true;
    }

    bool SqlAdapter::addGrade(Core::Types::Student& targetStudent, Core::Types::Subject& targetSubject, const float& grade)
    {
        LOG((*logger_), "Adding new grade from \"", targetSubject.name_, "\" to student ", targetStudent.firstName_ , " ", targetStudent.lastName_);
        Table targetTable = sManager_->getTableSchema("Grades");
        if(sManager_->addEntryToTable(targetTable.getName(),
            {std::make_pair(targetTable.getAttributeByName("grade"),     std::to_string(grade)),
             std::make_pair(targetTable.getAttributeByName("subjectId"), std::to_string(targetSubject.id_)),
             std::make_pair(targetTable.getAttributeByName("studentId"), std::to_string(targetStudent.id_))}))
        {
            LOG((*logger_), "Assigned grade (", grade, ") to a student [", targetStudent.email_, " ", targetStudent.id_, "] from a subject [", targetSubject.name_, " ", targetSubject.id_, "]");
            return true;
        }
        LOG((*logger_), "Could not assign given grade");
        return false;
    }

    bool SqlAdapter::removeGrade(const Core::Types::Student& targetStudent, const Core::Types::Subject& targetSubject)
    {
        LOG((*logger_), "Attempting to remove grade from \"", targetSubject.name_, "\" from student ", targetStudent.firstName_ , " ", targetStudent.lastName_);
        // TODO add handle
        return true;
    }

    bool SqlAdapter::addCourse(Course& newCourse)
    {
        LOG((*logger_), "Attempting to add new course ( name:", newCourse.name_, " minStudents:", newCourse.minStudents_, " maxStudents:", newCourse.maxStudents_ ,")");
        Table targetTable = sManager_->getTable("Courses");
        if(sManager_->addEntryToTable(targetTable.getName(), 
            {std::make_pair(targetTable.getAttributeByName("name"), newCourse.name_),
             std::make_pair(targetTable.getAttributeByName("baseMinimalPoints"), std::to_string(newCourse.baseMinimalPoints_)),
             std::make_pair(targetTable.getAttributeByName("minStudents"), std::to_string(newCourse.minStudents_)),
             std::make_pair(targetTable.getAttributeByName("maxStudents"), std::to_string(newCourse.maxStudents_))}))
        {
            newCourse.id_ = getLatestIdFromTable("Courses");
            LOG((*logger_), "New course added.");
            return true;
        }
        LOG((*logger_), "Could not addd new course");
        return false;
    }

    bool SqlAdapter::updateCourse(const Course& targetCourse, const Course& newCourse)
    {
        LOG((*logger_), "Attempting to modify course ( name:", targetCourse.name_, " minStudents:", targetCourse.minStudents_, " maxStudents:", targetCourse.maxStudents_ ,") <-> (name: ", newCourse.name_, " minStudents:", newCourse.minStudents_, " maxStudents:", newCourse.maxStudents_, ")");
        AttrsValues newValPacket{};
        // First compare 
        if(targetCourse.name_ != newCourse.name_)
        {
            LOG((*logger_), "Name mismatch ", targetCourse.name_, " <-> ", newCourse.name_);
            // For altering existing attribute there is no need to add any extra flags
            newValPacket.push_back(std::make_pair(Utilities::Sql::Types::Attribute("name"), newCourse.name_));
        }
        
        if(targetCourse.baseMinimalPoints_ != newCourse.baseMinimalPoints_)
        {
            LOG((*logger_), "BaseMinimalPoints mismatch ", targetCourse.baseMinimalPoints_, " <-> ", newCourse.baseMinimalPoints_);
            newValPacket.push_back(std::make_pair(Utilities::Sql::Types::Attribute("baseMinimalPoints"), std::to_string(newCourse.baseMinimalPoints_)));
        }

        if(targetCourse.minStudents_ != newCourse.minStudents_)
        {
            LOG((*logger_), "MinStudents mismatch ", targetCourse.minStudents_, "<->", newCourse.minStudents_);
            newValPacket.push_back(std::make_pair(Utilities::Sql::Types::Attribute("minStudents"), std::to_string(newCourse.minStudents_)));
        }

        if(targetCourse.maxStudents_ != newCourse.maxStudents_)
        {
            LOG((*logger_), "MaxStudents mismatch ", targetCourse.maxStudents_, "<->", newCourse.maxStudents_);
            newValPacket.push_back(std::make_pair(Utilities::Sql::Types::Attribute("maxStudents"), std::to_string(newCourse.maxStudents_)));
        }


        if(newValPacket.empty())
        {
            LOG((*logger_), "New Course = Old Course nothing needs to be altered");
            return true;
        }

        LOG((*logger_), "Changes were found, DB will be contacted");
        std::string condition = "id = " + std::to_string(targetCourse.id_);
        return sManager_->updateEntryFromTable("Courses", newValPacket, condition);
    }

    bool SqlAdapter::removeCourse(Course& targetCourse)
    {
        LOG((*logger_), "Attempting to remove the course: ", targetCourse.name_);
        if(sManager_->removeEntryFromTable("Courses", targetCourse.id_))
        {
            LOG((*logger_), "Course ", targetCourse.name_, " was deleted successfully");
            return true;
        }
        LOG((*logger_), "Course ", targetCourse.name_, " could not be deleted");
        return false;
    }

    bool SqlAdapter::addSrequest(Request::Srequest& newRequest)
    {
        LOG((*logger_), "Adding request from: ", newRequest.studentId_, " to: ", newRequest.courseId_);
        Table targetTable = sManager_->getTable("StudentRequest");
        if(sManager_->addEntryToTable(targetTable.getName(), 
            {std::make_pair(targetTable.getAttributeByName("courseId_"), std::to_string(newRequest.courseId_)),
             std::make_pair(targetTable.getAttributeByName("studentId_"), std::to_string(newRequest.studentId_))}))
        {
            LOG((*logger_), "New request has been added. From: ", newRequest.studentId_, " to ", newRequest.courseId_);
            return true;
        }
        LOG((*logger_), "Request from: ", newRequest.studentId_, " to: ", newRequest.courseId_, " could not be added");
        return false;
    }

    bool SqlAdapter::updateSrequest(const Request::Srequest& targetSreq, const Request::Srequest& newSreq)
    {
        LOG((*logger_), "Attempting to update a request from: ", targetSreq.studentId_, " to: ", targetSreq.courseId_);
        Utilities::Sql::AttrsValues newValPacket{};

        // First compare
        if(targetSreq.courseId_ != newSreq.courseId_)
        {
            LOG((*logger_), "Missmatch in courseId ", targetSreq.courseId_, " <-> ", newSreq.courseId_);
            newValPacket.push_back(std::make_pair(Utilities::Sql::Types::Attribute("courseId"), std::to_string(newSreq.courseId_)));
        }
    
        if(targetSreq.studentId_ != newSreq.studentId_)
        {
            LOG((*logger_), "Missmatch in studentId ", targetSreq.studentId_, " <-> ", newSreq.studentId_);
            newValPacket.push_back(std::make_pair(Utilities::Sql::Types::Attribute("studentId"), std::to_string(newSreq.studentId_)));
        }

        if(targetSreq.status_ != newSreq.status_)
        {
            LOG((*logger_), "Missmatch in status ", Request::statusToString(targetSreq.status_), " <-> ", Request::statusToString(newSreq.status_));
            newValPacket.push_back(std::make_pair(Utilities::Sql::Types::Attribute("status"), std::to_string(newSreq.status_)));
        }

        if(newValPacket.empty())
        {
            LOG((*logger_), "New SRequest = Old SRequest nothing needs to be altered");
            return true;
        }

        LOG((*logger_), "Changes were found, DB will be contacted");
        std::string condition = "id = " + std::to_string(targetSreq.requestId_);
        return sManager_->updateEntryFromTable("StudentRequest", newValPacket, condition);
    }

    bool SqlAdapter::removeSrequest(Request::Srequest& targetRequest)
    {
        LOG((*logger_), "Removing request from: ", targetRequest.studentId_, " to: ", targetRequest.courseId_);
        if(sManager_->removeEntryFromTable("StudentRequest", targetRequest.requestId_))
        {
            LOG((*logger_), "Request from :", targetRequest.studentId_, " to: ", targetRequest.courseId_, " was removed");
            return true;
        }
        LOG((*logger_), "Request from :", targetRequest.studentId_, " to: ", targetRequest.courseId_, " could not be removed");
        return false;
    }

} // namespace Utilities::Sql
