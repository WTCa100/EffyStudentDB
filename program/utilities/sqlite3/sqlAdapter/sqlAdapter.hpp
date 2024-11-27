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

#include "../../logger/logger.hpp"

namespace Utilities::Sql
{
    class SqlAdapter
    {
    private:
        std::shared_ptr<Logger> logger_;
        std::shared_ptr<SqlManager> sManager_;
    public:
        SqlAdapter(std::shared_ptr<Logger> logger, std::shared_ptr<SqlManager> sManager);
        ~SqlAdapter() = default;

        std::vector<Core::Types::School>            getSchools();
        std::vector<Core::Types::Student>           getStudents();
        std::vector<Core::Types::Subject>           getSubjects();
        std::vector<std::vector<std::string>>       getGrades();
        std::vector<Core::Types::Course>            getCourses();
        std::vector<Core::Types::Request::Srequest> getSrequests();

        void mapSubjectToCourseWeight(Core::Types::Course& targetCourse);        
        uint16_t getLatestIdFromTable(std::string tblName);

        bool addSchool(Core::Types::School& newSchool);
        bool removeSchool(const Core::Types::School& removeSchool);
        bool addStudent(Core::Types::Student& newStudent);
        bool removeStudent(const Core::Types::Student& targetStudent);
        bool addSubject(Core::Types::Subject& newSubject);
        bool removeSubject(const Core::Types::Subject& targetSubject);
        bool addGrade(Core::Types::Student& targetStudent, Core::Types::Subject& targetSubject, const float& grade);
        bool removeGrade(const Core::Types::Student& targetStudent, const Core::Types::Subject& targetSubject);
        bool addCourse(Core::Types::Course& newCourse);
        bool removeCourse(Core::Types::Course& targetCourse);
        bool addSrequest(Core::Types::Request::Srequest& newSrequest);
        bool removeSrequest(Core::Types::Request::Srequest& targetRequest);
    };
    
} // namespace Utilities


