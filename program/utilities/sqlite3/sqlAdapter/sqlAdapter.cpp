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
        std::vector<std::string> rawEntries = sManager_->getEntriesFromTable("CourseSubjectWeight", {"subjectId", "weight"}, ("courseId =" + std::to_string(targetCourse.id_)));
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

    bool SqlAdapter::addEntry(Entry& newEntry)
    {
        std::string targetTableName = newEntry.associatedTable_;
        LOG((*logger_), "Adding new entry \"", newEntry.toString(), " to table: ", targetTableName);
        Table targetTable = sManager_->getTableSchema(targetTableName);
        if(!targetTable.isValid())
        {
            LOG((*logger_), "Table ", targetTableName, " does not exist. Aborting procedure.");
            return false;
        }

        Utilities::Sql::AttrsValues entryAttrs;
        // Extract info about attributes 
        for(const auto& entryAttr : newEntry.getAttrs())
        {
            Utilities::Sql::Types::Attribute curAttr = targetTable.getAttributeByName(entryAttr.first);
            if(!curAttr.isValid())
            {
                LOG((*logger_), "Attribute ", entryAttr.first, " is not valid. Skipping...");
                continue;
            }
            LOG((*logger_), "Adding to attribute: ", entryAttr.first, " value: ", entryAttr.second);
            entryAttrs.push_back(std::make_pair(curAttr, entryAttr.second));
        }
        if(sManager_->addEntryToTable(targetTableName, entryAttrs))
        {
            newEntry.id_ = getLatestIdFromTable(targetTableName);
            LOG((*logger_), "New entry has been added to a table: ", targetTableName);
            return true;
        }
        LOG((*logger_), "Could not add entry to a table: ", targetTableName);
        return true;
    }

    bool SqlAdapter::updateEntry(const Entry& oldEntry, const Entry& newEntry)
    {
        std::string targetTableName = oldEntry.associatedTable_;
        LOG((*logger_), "Updating target entry: ", oldEntry.toString(), " Compare has started.");

        if(targetTableName != newEntry.associatedTable_)
        {
            LOG((*logger_), "Associated table is different in both entires. Cannot edit.");
            return false;
        }

        Table targetTable = sManager_->getTableSchema(targetTableName);
        if(!targetTable.isValid())
        {
            LOG((*logger_), "Table ", targetTableName, " does not exist. Aborting procedure.");
            return false;
        }

        std::map<std::string, std::string> entryAttrsOld, entryAttrsNew;
        AttrsValues newValPacket{};
        entryAttrsOld = oldEntry.getAttrs();
        entryAttrsNew = newEntry.getAttrs();

        for(const auto& entryComp : entryAttrsOld)
        {
            std::string newVal = entryAttrsNew.at(entryComp.first);
            if(newVal != entryComp.second)
            {
                LOG((*logger_), entryComp.first, " mismatch: ", entryComp.second , " <-> ", newVal);
                newValPacket.push_back(std::make_pair(Utilities::Sql::Types::Attribute(entryComp.first), newVal));
            }
        }

        if(newValPacket.empty())
        {
            LOG((*logger_), "Old Entry = New Entry nothing needs to be altered");
            return true;
        }

        LOG((*logger_), "Changes were found, DB will be contacted");
        std::string condition = "id = " + std::to_string(newEntry.id_);
        return sManager_->updateEntryFromTable(targetTableName, newValPacket, condition);
    }

    bool SqlAdapter::removeEntry(const Entry& targetEntry)
    {
        std::string targetTableName = targetEntry.associatedTable_;
        LOG((*logger_), "Attempting to remove entry: ", targetEntry.toString(), " from table: ", targetTableName);
    
        Table targetTable = sManager_->getTableSchema(targetTableName);
        if(!targetTable.isValid())
        {
            LOG((*logger_), "Table ", targetTableName, " does not exist. Aborting procedure.");
            return false;
        }

        sManager_->removeEntryFromTable(targetTableName, targetEntry.id_);
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

} // namespace Utilities::Sql
