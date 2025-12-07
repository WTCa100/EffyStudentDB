#include "entryFactory.hpp"

#include "course/course.hpp"
#include "courseSubjectWeight/courseSubjectWeight.hpp"
#include "grade/grade.hpp"
#include "school/school.hpp"
#include "student/student.hpp"
#include "studentRequest/request.hpp"
#include "subject/subject.hpp"

namespace Core::Types
{
    std::shared_ptr<Entry> EntryFactory::getEntry(const std::string& tableName)
    {
        using namespace Utilities::Common::Constants;
        std::shared_ptr<Entry> tmp;
        // Extract concrete type
        if (tableName == g_tableSchools) { tmp = std::make_shared<School>(); }
        else if (tableName == g_tableStudents) { tmp = std::make_shared<Student>(); }
        else if (tableName == g_tableSubjects) { tmp = std::make_shared<Subject>(); }
        else if (tableName == g_tableCourses) { tmp = std::make_shared<Course>(); }
        else if (tableName == g_tableGrades) { tmp = std::make_shared<Grade>(); }
        else if (tableName == g_tableCourseSubjectWeight) { tmp = std::make_shared<CourseSubjectWeight>(); }
        else { tmp = std::make_shared<Core::Types::Request::Srequest>(); }
        return tmp;
    }
}  // namespace Core::Types
