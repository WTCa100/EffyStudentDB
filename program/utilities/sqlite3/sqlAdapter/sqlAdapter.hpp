#pragma once

#include <memory>
#include <filesystem>

// Types
#include "../sqlManager/sqlManager.hpp"
#include "../sqlManager/types/attribute/attribute.hpp"
#include "../sqlManager/types/table/table.hpp"
#include "../../../types/school/school.hpp"
#include "../../../types/student/student.hpp"
#include "../../../types/subject/subject.hpp"
#include "../../../types/course/course.hpp"
#include "../../../types/studentRequest/request.hpp"
#include "../../../types/entry.hpp"

#include "../../logger/logger.hpp"

namespace Utilities::Sql
{
    using namespace Core::Types;
    class SqlAdapter
    {
    private:
        std::shared_ptr<Logger> logger_;
        std::shared_ptr<SqlManager> sManager_;
    public:
        SqlAdapter(std::shared_ptr<Logger> logger, std::shared_ptr<SqlManager> sManager);
        ~SqlAdapter() = default;

        std::vector<School>                         getSchools(std::string filter = "");
        std::vector<Student>                        getStudents(std::string filter = "");
        std::vector<Subject>                        getSubjects(std::string filter = "");
        std::vector<std::vector<std::string>>       getGrades(std::string filter = "");
        std::vector<Course>                         getCourses(std::string filter = "");
        std::vector<Request::Srequest>              getSrequests(std::string filter = "");
        std::vector<std::shared_ptr<Entry>>         getEntries(std::string tableName, std::string filter = "");

        void mapSubjectToCourseWeight(Course& targetCourse);        
        uint16_t getLatestIdFromTable(std::string tblName);

        bool addEntry(Entry& newEntry);
        bool updateEntry(const Entry& oldEntry, const Entry& newEntry);
        bool removeEntry(const Entry& targetEntry);
        bool addGrade(Student& targetStudent, Subject& targetSubject, const float& grade);
        bool removeGrade(const Student& targetStudent, const Subject& targetSubject);

        std::string makeFilter(std::unordered_map<std::string, std::string> attrs);
    };
    
} // namespace Utilities


