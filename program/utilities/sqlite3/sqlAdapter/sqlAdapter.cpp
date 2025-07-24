#include "sqlAdapter.hpp"

#include "../../common/constants.hpp"
#include "../../common/stringManip.hpp"

using Utilities::Sql::Types::Table;
using namespace Utilities::Common::Constants;

namespace Utilities::Sql
{
    SqlAdapter::SqlAdapter(std::shared_ptr<Logger> logger, std::shared_ptr<SqlManager> sManager):
        logger_(logger),
        sManager_(sManager)
    {
        LOG((*logger_), "Creation of SqlAdapter");
    }

    /// @brief This function is used to get entries in an abstract way, allowing polymorphism.
    /// @param tableName target table
    /// @param filter every entry if empty
    /// @return Vector of Entries from a concrete class
    std::vector<std::shared_ptr<Entry>> SqlAdapter::getEntries(std::string tableName, std::string filter)
    {
        LOG((*logger_), "Getting abstract objects from table: ", tableName, " with filter: \"", filter, "\"");
        std::vector<std::shared_ptr<Entry>> entries;
        if (tableName == g_tableSchools)
        {
            std::vector<School> concreteEntries = getSchools(filter);
            for (const auto& val : concreteEntries) { entries.push_back(std::make_shared<School>(val)); }
        }

        if (tableName == g_tableStudents)
        {
            std::vector<Student> concreteEntries = getStudents(filter);
            for (const auto& val : concreteEntries) { entries.push_back(std::make_shared<Student>(val)); }
        }

        if (tableName == g_tableSubjects)
        {
            std::vector<Subject> concreteEntries = getSubjects(filter);
            for (const auto& val : concreteEntries) { entries.push_back(std::make_shared<Subject>(val)); }
        }

        if (tableName == g_tableGrades)
        {
            std::vector<Grade> concreteEntries = getGrades(filter);
            for (const auto& val : concreteEntries) { entries.push_back(std::make_shared<Grade>(val)); }
        }

        if (tableName == g_tableCourses)
        {
            std::vector<Course> concreteEntries = getCourses(filter);
            for (const auto& val : concreteEntries) { entries.push_back(std::make_shared<Course>(val)); }
        }

        if (tableName == g_tableCourseSubjectWeight)
        {
            std::vector<CourseSubjectWeight> concreteEntries = getCourseSubjectWeight(filter);
            for (const auto& val : concreteEntries) { entries.push_back(std::make_shared<CourseSubjectWeight>(val)); }
        }
        LOG((*logger_), "Returning entries of size n=", entries.size());
        return entries;
    }

    std::vector<Core::Types::School> SqlAdapter::getSchools(std::string filter)
    {
        std::vector<Core::Types::School> schools;
        LOG((*logger_), "Fetching schools from the SQL DB");
        std::vector<std::string> rawEntries = sManager_->getEntriesFromTable(g_tableSchools, { "id", "name" }, filter);
        if (rawEntries.empty())
        {
            LOG((*logger_), "Called schools, but got no entries!");
            return {};
        }

        LOG((*logger_), "Got n=", rawEntries.size(), " school entries");
        for (auto e : rawEntries)
        {
            std::vector<std::string> tokenizedSchool = Utilities::Common::tokenize(e, '|');
            // Tokens are:
            // (0)Id | (1)Name
            schools.push_back(
                Core::Types::School{ static_cast<uint16_t>(std::stoul(tokenizedSchool.at(0))), tokenizedSchool.at(1), {} });
        }
        LOG((*logger_), "Schools tokenized and pushed into the list. Final vector size = ", schools.size(),
            " Raw entries size = ", rawEntries.size());
        return schools;
    }

    std::vector<Core::Types::Student> SqlAdapter::getStudents(std::string filter)
    {
        std::vector<Core::Types::Student> students;
        LOG((*logger_), "Fetching students from the SQL DB");
        std::vector<std::string> rawEntries = sManager_->getEntriesFromTable(
            g_tableStudents, { "id", "firstName", "secondName", "lastName", "email", "schoolId" }, filter);
        if (rawEntries.empty())
        {
            LOG((*logger_), "Called students, but got no entries!");
            return {};
        }

        LOG((*logger_), "Got n=", rawEntries.size(), " student entries");
        for (auto e : rawEntries)
        {
            std::vector<std::string> tokenizedStudent = Utilities::Common::tokenize(e, '|');
            // Tokens are:
            // (0) ID | (1) FIRSTNAME | (2) SECONDNAME | (3) LASTNAME | (4) EMAIL | (5) SCHOOLID
            std::optional<std::string> secondName = std::nullopt;
            if (tokenizedStudent.at(2) != "NULL") { secondName = tokenizedStudent.at(2); }

            Core::Types::Student tmpStudent;
            tmpStudent.id_         = std::stoul(tokenizedStudent.at(0));
            tmpStudent.firstName_  = tokenizedStudent.at(1);
            tmpStudent.secondName_ = secondName;
            tmpStudent.lastName_   = tokenizedStudent.at(3);
            tmpStudent.email_      = tokenizedStudent.at(4);
            tmpStudent.grades_     = {};
            tmpStudent.schoolId_   = std::stoul(tokenizedStudent.at(5));
            students.push_back(tmpStudent);
        }
        LOG((*logger_), "Students tokenized and pushed into the list. Final vector size = ", students.size(),
            " Raw entries size = ", rawEntries.size());
        return students;
    }

    std::vector<Core::Types::Subject> SqlAdapter::getSubjects(std::string filter)
    {
        std::vector<Core::Types::Subject> subjects;
        LOG((*logger_), "Fetching subjects from the SQL DB");
        std::vector<std::string> rawEntries = sManager_->getEntriesFromTable(g_tableSubjects, { "id", "name" }, filter);
        if (rawEntries.empty())
        {
            LOG((*logger_), "Called subjects, but got no entries!");
            return {};
        }

        LOG((*logger_), "Got n=", rawEntries.size(), " subjects entries");
        for (auto e : rawEntries)
        {
            std::vector<std::string> tokenizedSubjects = Utilities::Common::tokenize(e, '|');
            // Tokens are:
            // (0)Id | (1)Name
            subjects.push_back(
                Core::Types::Subject{ static_cast<uint16_t>(std::stoul(tokenizedSubjects.at(0))), tokenizedSubjects.at(1) });
        }
        LOG((*logger_), "Subjects tokenized and pushed into the list. Final vector size = ", subjects.size(),
            " Raw entries size = ", rawEntries.size());
        return subjects;
    }

    std::vector<Grade> SqlAdapter::getGrades(std::string filter)
    {
        LOG((*logger_), "Fetching grades from the SQL DB");
        std::vector<std::string> rawEntries =
            sManager_->getEntriesFromTable(g_tableGrades, { "id", "grade", "studentId", "subjectId" }, filter);
        if (rawEntries.empty())
        {
            LOG((*logger_), "Called grades, but got no entries!");
            return {};
        }

        std::vector<Grade> listOfGrades;
        LOG((*logger_), "Got n=", rawEntries.size(), " grades entries");
        for (auto e : rawEntries)
        {
            std::vector<std::string> tokenizedGrade = Utilities::Common::tokenize(e, '|');
            Grade tmpGrade;
            tmpGrade.id_          = std::stoul(tokenizedGrade.at(0));
            tmpGrade.value_       = std::stold(tokenizedGrade.at(1));
            tmpGrade.studentId_   = std::stoul(tokenizedGrade.at(2));
            tmpGrade.studentName_ = "";
            tmpGrade.subjectId_   = std::stoul(tokenizedGrade.at(3));
            tmpGrade.subjectName_ = "";
            listOfGrades.push_back(tmpGrade);
        }

        LOG((*logger_), "Grades tokenized and pushed into the list. Final vector size = ", listOfGrades.size(),
            " Raw entries size = ", rawEntries.size());
        return listOfGrades;
    }

    std::vector<CourseSubjectWeight> SqlAdapter::getCourseSubjectWeight(std::string filter)
    {
        std::vector<CourseSubjectWeight> weights;
        LOG((*logger_), "Fetching CourseSubjectWeights from the SQL DB");
        std::vector<std::string> rawEntries =
            sManager_->getEntriesFromTable(g_tableCourseSubjectWeight, { "id", "weight", "subjectId", "courseId" }, filter);
        if (rawEntries.empty())
        {
            LOG((*logger_), "Called CourseSubjectWeight, but got no entries")
            return {};
        }

        LOG((*logger_), "Got n=", rawEntries.size(), " entries");
        for (auto e : rawEntries)
        {
            std::vector<std::string> tokenizedWeight = Utilities::Common::tokenize(e, '|');
            Core::Types::CourseSubjectWeight tmpWeight;
            tmpWeight.id_        = std::stoul(tokenizedWeight.at(0));
            tmpWeight.weight_    = std::stod(tokenizedWeight.at(1));
            tmpWeight.subjectId_ = std::stoul(tokenizedWeight.at(2));
            tmpWeight.courseId_  = std::stoul(tokenizedWeight.at(3));

            weights.push_back(tmpWeight);
        }
        LOG((*logger_), "CourseSubjectWeight tokenized and pushed into the list. Final vector size = ", weights.size(),
            " Raw entries size = ", rawEntries.size());
        return weights;
    }

    std::vector<Core::Types::Course> SqlAdapter::getCourses(std::string filter)
    {
        std::vector<Core::Types::Course> courses;
        LOG((*logger_), "Fetching courses from the SQL DB");
        std::vector<std::string> rawEntries = sManager_->getEntriesFromTable(
            g_tableCourses, { "id", "minStudents", "maxStudents", "baseMinimalPoints", "name", "isOpen", "recrutingTurn"}, filter);
        if (rawEntries.empty())
        {
            LOG((*logger_), "Called courses, but got no entries!");
            return {};
        }

        LOG((*logger_), "Got n=", rawEntries.size(), " entries");
        for (auto e : rawEntries)
        {
            std::vector<std::string> tokenizedCourse = Utilities::Common::tokenize(e, '|');
            // Tokens are:
            // (0) id | (1) minStudentCount | (2) maxStudentsCount | (3) baseMinimalPoints | (4) avgStudentPoints | (5) name |
            // (6) subjectWithWeight | (7) isOpen | (8) recrutingTurn
            Core::Types::Course tmpCourse;
            tmpCourse.id_                   = std::stoul(tokenizedCourse.at(0));
            tmpCourse.minStudents_          = std::stoi(tokenizedCourse.at(1));
            tmpCourse.maxStudents_          = std::stoi(tokenizedCourse.at(2));
            tmpCourse.baseMinimalPoints_    = std::stoi(tokenizedCourse.at(3));
            tmpCourse.averageStudentPoints_ = tmpCourse.baseMinimalPoints_;
            tmpCourse.name_                 = tokenizedCourse.at(4);
            tmpCourse.subjectWithWeight_    = {};
            tmpCourse.isOpen_ = static_cast<OpenState>(std::stoi(tokenizedCourse.at(5)));
            tmpCourse.recrutingTurn_ = std::stoi(tokenizedCourse.at(6));

            courses.push_back(std::move(tmpCourse));  // It's quite big move makes more sense here
        }
        LOG((*logger_), "Courses tokenized and pushed into the list. Final vector size = ", courses.size(),
            " Raw entries size = ", rawEntries.size());
        return courses;
    }

    std::vector<Core::Types::Request::Srequest> SqlAdapter::getSrequests(std::string filter)
    {
        std::vector<Core::Types::Request::Srequest> sRequests;
        LOG((*logger_), "Fetching student requests from the SQL DB");
        std::vector<std::string> rawEntries =
            sManager_->getEntriesFromTable(g_tableStudentRequest, { "id", "studentId", "courseId", "requestStatus" }, filter);

        if (rawEntries.empty())
        {
            LOG((*logger_), "Called student requests, but got no entries!");
            return {};
        }

        LOG((*logger_), "Got n=", rawEntries.size(), " entries");
        for (auto e : rawEntries)
        {
            std::vector<std::string> tokenizedEntry = Utilities::Common::tokenize(e, '|');
            // Tokens are
            // (0) RequestId | (1) studentId | (2) studentId | (3) requestStatus
            Request::Srequest newEntry(static_cast<uint32_t>(std::stoul(tokenizedEntry.at(0))),
                static_cast<uint16_t>(std::stoul(tokenizedEntry.at(1))), static_cast<uint16_t>(std::stoul(tokenizedEntry.at(2))),
                static_cast<Request::requestStatus>(std::stoul(tokenizedEntry.at(3))));
            sRequests.push_back(newEntry);
        }

        LOG((*logger_), "Courses tokenized and pushed into the list. Final vector size = ", sRequests.size(),
            " Raw entries size = ", rawEntries.size());
        return sRequests;
    }

    uint16_t SqlAdapter::getLatestIdFromTable(std::string tblName)
    {
        LOG((*logger_), "Getting latest id from ", tblName);
        std::vector<std::string> result = sManager_->getEntriesFromTable(tblName, { "MAX(id)" });
        // Check if it's empty - such scenario would probably never occur?
        LOG((*logger_), "Max ID from table = ", std::stoul(result.at(0)));
        return static_cast<uint16_t>(std::stoul(result.at(0)));
    }

    bool SqlAdapter::addEntry(Entry& newEntry)
    {
        std::string targetTableName = newEntry.associatedTable_;
        LOG((*logger_), "Adding new entry \"", newEntry.toString(), " to table: ", targetTableName);
        Table targetTable = sManager_->getTableSchema(targetTableName);
        if (!targetTable.isValid())
        {
            LOG((*logger_), "Table ", targetTableName, " does not exist. Aborting procedure.");
            return false;
        }

        Utilities::Sql::AttrsValues entryAttrs;
        // Extract info about attributes
        for (const auto& entryAttr : newEntry.getAttrs())
        {
            Utilities::Sql::Types::Attribute curAttr = targetTable.getAttributeByName(entryAttr.first);
            if (!curAttr.isValid())
            {
                LOG((*logger_), "Attribute ", entryAttr.first, " is not valid. Skipping...");
                continue;
            }
            LOG((*logger_), "Adding to attribute: ", entryAttr.first, " value: ", entryAttr.second);
            entryAttrs.push_back(std::make_pair(curAttr, entryAttr.second));
        }
        if (sManager_->addEntryToTable(targetTableName, entryAttrs))
        {
            newEntry.id_ = getLatestIdFromTable(targetTableName);
            LOG((*logger_), "New entry has been added to a table: ", targetTableName);
            return true;
        }
        LOG((*logger_), "Could not add entry to a table: ", targetTableName);
        return false;
    }

    bool SqlAdapter::updateEntry(const Entry& oldEntry, const Entry& newEntry)
    {
        std::string targetTableName = oldEntry.associatedTable_;
        LOG((*logger_), "Updating target entry: ", oldEntry.toString(), " Compare has started.");

        if (targetTableName != newEntry.associatedTable_)
        {
            LOG((*logger_), "Associated table is different in both entires. Cannot edit.");
            return false;
        }

        Table targetTable = sManager_->getTableSchema(targetTableName);
        if (!targetTable.isValid())
        {
            LOG((*logger_), "Table ", targetTableName, " does not exist. Aborting procedure.");
            return false;
        }

        std::map<std::string, std::string> entryAttrsOld, entryAttrsNew;
        AttrsValues newValPacket{};
        entryAttrsOld = oldEntry.getAttrs();
        entryAttrsNew = newEntry.getAttrs();

        for (const auto& entryComp : entryAttrsOld)
        {
            std::string newVal = entryAttrsNew.at(entryComp.first);
            if (newVal != entryComp.second)
            {
                LOG((*logger_), entryComp.first, " mismatch: ", entryComp.second, " <-> ", newVal);
                newValPacket.push_back(std::make_pair(
                    Utilities::Sql::Types::Attribute(entryComp.first, Types::attrFlagToString(Types::AttributeFlag::MISSING), {}),
                    newVal));
            }
        }

        if (newValPacket.empty())
        {
            LOG((*logger_), "Old Entry = New Entry nothing needs to be altered");
            return true;
        }

        LOG((*logger_), "Changes were found, DB will be contacted");
        std::string condition = "id = " + std::to_string(oldEntry.id_);
        return sManager_->updateEntryFromTable(targetTableName, newValPacket, condition);
    }

    bool SqlAdapter::removeEntry(const Entry& targetEntry, std::optional<std::string> condition)
    {
        std::string targetTableName = targetEntry.associatedTable_;
        LOG((*logger_), "Attempting to remove entry: ", targetEntry.toString(), " from table: ", targetTableName);

        Table targetTable = sManager_->getTableSchema(targetTableName);
        if (!targetTable.isValid())
        {
            LOG((*logger_), "Table ", targetTableName, " does not exist. Aborting procedure.");
            return false;
        }

        if (condition.has_value()) { return sManager_->removeEntryFromTable(targetTableName, condition.value()); }
        return sManager_->removeEntryFromTable(targetTableName, targetEntry.id_);
    }

    bool SqlAdapter::addGrade(Core::Types::Student& targetStudent, Core::Types::Subject& targetSubject, const float& grade)
    {
        LOG((*logger_), "Adding new grade from \"", targetSubject.name_, "\" to student ", targetStudent.firstName_, " ",
            targetStudent.lastName_);
        Table targetTable = sManager_->getTableSchema(g_tableGrades);
        if (sManager_->addEntryToTable(targetTable.getName(),
                { std::make_pair(targetTable.getAttributeByName("grade"), std::to_string(grade)),
                    std::make_pair(targetTable.getAttributeByName("subjectId"), std::to_string(targetSubject.id_)),
                    std::make_pair(targetTable.getAttributeByName("studentId"), std::to_string(targetStudent.id_)) }))
        {
            LOG((*logger_), "Assigned grade (", grade, ") to a student [", targetStudent.email_, " ", targetStudent.id_,
                "] from a subject [", targetSubject.name_, " ", targetSubject.id_, "]");
            return true;
        }
        LOG((*logger_), "Could not assign given grade");
        return false;
    }

    bool SqlAdapter::removeGrade(const Core::Types::Student& targetStudent, const Core::Types::Subject& targetSubject)
    {
        LOG((*logger_), "Attempting to remove grade from \"", targetSubject.name_, "\" from student ", targetStudent.firstName_,
            " ", targetStudent.lastName_);
        std::stringstream condition;
        condition << "studentId = " << targetStudent.id_ << " AND subjectId = " << targetSubject.id_;
        if (sManager_->removeEntryFromTable("Grades", condition.str()))
        {
            LOG((*logger_), "Removed grade from a student [", targetStudent.email_, " ", targetStudent.id_, "] from a subject [",
                targetSubject.name_, " ", targetSubject.id_, "]");
        }
        LOG((*logger_), "Could not remove given grade: subjectId = ", targetSubject.id_, " studentId = ", targetStudent.id_,
            " email = ", targetStudent.email_);
        return false;
    }

    std::string SqlAdapter::makeFilter(std::unordered_map<std::string, std::string> attrs, bool exact)
    {
        std::stringstream filter;
        size_t currentAttr    = 0;
        const size_t attrLast = attrs.size() - 1;
        for (const auto& attr : attrs)
        {
            if (exact) { filter << attr.first << " = '" << attr.second << "' "; }
            else { filter << attr.first << " LIKE '%" << attr.second << "%' "; }

            if (currentAttr != attrLast) filter << "AND ";
            ++currentAttr;
        }

        return filter.str();
    }

    std::vector<std::pair<uint16_t, uint16_t>> SqlAdapter::getAttendees()
    {
        std::vector<std::pair<uint16_t, uint16_t>> attendees;
        LOG((*logger_), "Fetching Attendees from the SQL DB");
        std::vector<std::string> rawEntries = sManager_->getEntriesFromTable(g_tableCourseAttendees, { "studentId", "courseId" });
        if (rawEntries.empty())
        {
            LOG((*logger_), "Called CourseAttendees but got no entries!");
            return {};
        }

        LOG((*logger_), "Got n=", rawEntries.size(), " student entries");
        for (auto entry : rawEntries)
        {
            std::vector<std::string> tokenizedEntries = Utilities::Common::tokenize(entry, '|');
            uint16_t studentId                        = std::stoul(tokenizedEntries.at(0));
            uint16_t courseId                         = std::stoul(tokenizedEntries.at(1));
            attendees.push_back(std::make_pair(studentId, courseId));
        }
        LOG((*logger_), "Course attendees tokenized and pushed into the list. Final vector size = ", attendees.size(),
            " Raw entries size = ", rawEntries.size());
        return attendees;
    }

    bool SqlAdapter::addAttendee(const uint16_t& studentId, const uint16_t& courseId)
    {
        LOG((*logger_), "Adding attendee ", studentId, " to course ", courseId);
        Table targetTable = sManager_->getTableSchema(g_tableCourseAttendees);
        if (sManager_->addEntryToTable(targetTable.getName(),
                { std::make_pair(targetTable.getAttributeByName("studentId"), std::to_string(studentId)),
                    std::make_pair(targetTable.getAttributeByName("courseId"), std::to_string(courseId)) }))
        {
            LOG((*logger_), "Attendee added successfully");
            return true;
        }
        LOG((*logger_), "Could not add attendee");
        return false;
    }

    bool SqlAdapter::removeAttendee(const uint16_t& studentId, const uint16_t& courseId)
    {
        LOG((*logger_), "Removing attendee ", studentId, " from course ", courseId);
        Table targetTable = sManager_->getTableSchema(g_tableCourseAttendees);
        std::unordered_map<std::string, std::string> attrs;
        attrs.insert(std::make_pair("studentId", std::to_string(studentId)));
        attrs.insert(std::make_pair("courseId", std::to_string(courseId)));
        std::string filter = makeFilter(attrs);
        if (sManager_->removeEntryFromTable(targetTable.getName(), filter))
        {
            LOG((*logger_), "Successfully deleted attendee");
            return true;
        }
        LOG((*logger_), "Could not delete attendee");
        return false;
    }

    bool SqlAdapter::changeCourseOpenState(const uint16_t& courseId, const std::string& newState)
    {
        Table targetTable = sManager_->getTableSchema(g_tableCourses);
        Utilities::Sql::AttrsValues state = {std::make_pair(targetTable.getAttributeByName("isOpen"), newState)};
        std::string condition = "id = " + std::to_string(courseId);
        return sManager_->updateEntryFromTable(g_tableCourses, state, condition);
    }


    bool SqlAdapter::openCourse(const uint16_t& courseId)
    {
        LOG((*logger_), "Opening course: ", courseId);
        return changeCourseOpenState(courseId, std::to_string(static_cast<uint8_t>(OpenState::opened)));
    }

    bool SqlAdapter::closeCourse(const uint16_t& courseId)
    {
        LOG((*logger_), "Closing course: ", courseId);
        return changeCourseOpenState(courseId, std::to_string(static_cast<uint8_t>(OpenState::closed)));
    }

}  // namespace Utilities::Sql
