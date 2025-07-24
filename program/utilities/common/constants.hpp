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
}  // namespace Utilities::Common::Constants
