#pragma once

#include <string>

namespace Utilities::Common::Constants
{
    // For standardized table name usage
    const std::string g_tableSchools             = "SCHOOLS";
    const std::string g_tableStudents            = "STUDENTS";
    const std::string g_tableSubjects            = "SUBJECTS";
    const std::string g_tableGrades              = "GRADES";
    const std::string g_tableCourses             = "COURSES";
    const std::string g_tableCourseSubjectWeight = "COURSESUBJECTWEIGHT";
    const std::string g_tableStudentRequest      = "STUDENTREQUEST";
    const std::string g_tableCourseAttendees     = "COURSEATTENDEES";

    constexpr uint16_t g_inputMissingValue = UINT16_MAX;

    enum class OpenState
    {
        notSet = 0,
        opened = 1,
        closed = 2
    };

    enum class AttendeeValuePosition
    {
        studentId = 0,
        courseId  = 1,
        points    = 2
    };

    // Min must become the greatest number here - given that the maximum weight can be 2.0 and at most 3 subjects can be present.
    // The maximum possible score would be: 2.0 * 10 * 3 = 60.0
    constexpr double maxPossibleScore = 60.0f;

}  // namespace Utilities::Common::Constants
