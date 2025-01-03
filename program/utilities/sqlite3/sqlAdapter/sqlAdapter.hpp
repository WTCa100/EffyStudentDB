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

        std::vector<School>                         getSchools();
        std::vector<Student>                        getStudents();
        std::vector<Subject>                        getSubjects();
        std::vector<std::vector<std::string>>       getGrades();
        std::vector<Course>                         getCourses();
        std::vector<Request::Srequest>              getSrequests();

        void mapSubjectToCourseWeight(Course& targetCourse);        
        uint16_t getLatestIdFromTable(std::string tblName);

        bool addEntry(Entry& newEntry);
        bool updateEntry(const Entry& oldEntry, const Entry& newEntry);
        bool removeEntry(const Entry& targetEntry);
        bool addGrade(Student& targetStudent, Subject& targetSubject, const float& grade);
        bool removeGrade(const Student& targetStudent, const Subject& targetSubject);
    };
    
} // namespace Utilities


