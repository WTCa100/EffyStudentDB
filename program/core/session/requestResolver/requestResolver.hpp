#pragma once

#include "../../../types/course/course.hpp"
#include "../../../types/courseSubjectWeight/courseSubjectWeight.hpp"
#include "../../../types/grade/grade.hpp"
#include "../../../types/student/student.hpp"
#include "../../../types/studentRequest/request.hpp"
#include "../../../utilities/logger/logger.hpp"
#include "../../../utilities/sqlite3/sqlAdapter/sqlAdapter.hpp"
#include "../data/data.hpp"

#include <map>
#include <memory>
#include <vector>

namespace Core
{
    using Utilities::Logger;
    using Utilities::Sql::SqlAdapter;

    using namespace Core::Types;
    typedef std::pair<Student, std::vector<Grade>> studentWithGrades;
    typedef std::pair<Course, std::vector<CourseSubjectWeight>> courseWithWeights;

    class RequestResolver
    {
      private:
        std::shared_ptr<SessionData> sesData_;
        std::shared_ptr<Logger> logger_;
        std::shared_ptr<SqlAdapter> SqlAdapter_;

        std::vector<Types::Request::Srequest> pendingRequests_;

        std::map<uint16_t, courseWithWeights> loadTargetedCoursesWithWeights(const std::vector<uint16_t>& coursesIds) const;
        std::map<uint16_t, studentWithGrades> loadInvokingStudentsWithGrades(const std::vector<uint16_t>& studentIds) const;
        std::vector<uint16_t> extractCourseIds() const;
        std::vector<uint16_t> extractStudentIds() const;
        bool handleRequest(studentWithGrades invoker, courseWithWeights target);

      public:
        RequestResolver(
            std::shared_ptr<Logger> logger, std::shared_ptr<SqlAdapter> sqlAdapter, std::shared_ptr<SessionData> sesData);
        ~RequestResolver() = default;
        void run();
        // void resolveRequest()
    };

}  // namespace Core
