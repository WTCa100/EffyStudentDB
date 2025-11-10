#pragma once

#include <filesystem>
#include <memory>

// Types
#include "../../../types/course/course.hpp"
#include "../../../types/courseSubjectWeight/courseSubjectWeight.hpp"
#include "../../../types/entry.hpp"
#include "../../../types/grade/grade.hpp"
#include "../../../types/school/school.hpp"
#include "../../../types/student/student.hpp"
#include "../../../types/studentRequest/request.hpp"
#include "../../../types/subject/subject.hpp"
#include "../../logger/logger.hpp"
#include "../sqlManager/sqlManager.hpp"
#include "../sqlManager/types/attribute/attribute.hpp"
#include "../sqlManager/types/table/table.hpp"

namespace Utilities::Sql
{
    using namespace Core::Types;
    typedef std::tuple<uint16_t, uint16_t, double> rawAttendee;
    constexpr bool filterExactMatch = 1;
    constexpr bool filterAnyMatch   = 0;

    class SqlAdapter
    {
      private:
        std::shared_ptr<Logger> logger_;
        std::shared_ptr<SqlManager> sManager_;

      public:
        SqlAdapter(std::shared_ptr<Logger> logger, std::shared_ptr<SqlManager> sManager);
        ~SqlAdapter() = default;

        std::vector<School> getSchools(std::string filter = "");
        std::vector<Student> getStudents(std::string filter = "");
        std::vector<Subject> getSubjects(std::string filter = "");
        std::vector<Grade> getGrades(std::string filter = "");
        std::vector<CourseSubjectWeight> getCourseSubjectWeight(std::string filter = "");
        std::vector<Course> getCourses(std::string filter = "");
        std::vector<Request::Srequest> getSrequests(std::string filter = "");
        std::vector<std::shared_ptr<Entry>> getEntries(std::string tableName, std::string filter = "");
        std::vector<rawAttendee> getAttendees(std::string filter = "");

        uint16_t getLatestIdFromTable(std::string tblName);

        bool addEntry(Entry& newEntry);
        bool updateEntry(const Entry& oldEntry, const Entry& newEntry);
        bool removeEntry(const Entry& targetEntry, std::optional<std::string> condition = std::nullopt);
        bool addGrade(Student& targetStudent, Subject& targetSubject, const float& grade);
        bool removeGrade(const Student& targetStudent, const Subject& targetSubject);

        bool addAttendee(const uint16_t& studentId, const uint16_t& courseId, const double& poitns = 100.0);
        bool removeAttendee(const uint16_t& studentId, const uint16_t& courseId);
        bool openCourse(const uint16_t& courseId);
        bool closeCourse(const uint16_t& courseId);
        bool changeCourseOpenState(const uint16_t& courseId, const std::string& newState);
        bool updateRequestStatus(const std::vector<uint16_t>& requestIds, const Request::requestStatus& newStatus);

        std::string makeFilter(std::unordered_map<std::string, std::string> attrs, bool exact = filterExactMatch);
        Request::requestStatus translate_to_status(const std::string& columnValue);
    };

}  // namespace Utilities::Sql

