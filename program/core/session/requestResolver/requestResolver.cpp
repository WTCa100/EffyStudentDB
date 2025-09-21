#include "requestResolver.hpp"

#include <sstream>

namespace Core
{
    RequestResolver::RequestResolver(
        std::shared_ptr<Logger> logger, std::shared_ptr<SqlAdapter> sqlAdapter, std::shared_ptr<SessionData> sesData):
        sesData_(sesData),
        logger_(logger),
        SqlAdapter_(sqlAdapter),
        pendingRequests_({})
    {
        std::string pendingRequestsOnly = "requestStatus = \"Pending\"";
        pendingRequests_                = SqlAdapter_->getSrequests(pendingRequestsOnly);
        LOG((*logger_), "Initialized request resolver with ", pendingRequests_.size(), " student's requests");
    }

    std::vector<uint16_t> RequestResolver::extractCourseIds() const
    {
        LOG((*logger_), "Getting course ids from requests");
        std::vector<uint16_t> coursesIds;
        for (const auto& request : pendingRequests_) { coursesIds.push_back(request.courseId_); }
        auto lastUnique = std::unique(coursesIds.begin(), coursesIds.end());  // TODO verify how to easily remove dups
        coursesIds.erase(lastUnique, coursesIds.end());
        return coursesIds;
    }

    std::vector<uint16_t> RequestResolver::extractStudentIds() const
    {
        LOG((*logger_), "Getting student ids from requests");
        std::vector<uint16_t> studentIds;
        for (const auto& request : pendingRequests_) { studentIds.push_back(request.studentId_); }
        auto lastUnique = std::unique(studentIds.begin(), studentIds.end());
        studentIds.erase(lastUnique, studentIds.end());
        return studentIds;
    }

    std::map<uint16_t, courseWithWeights> RequestResolver::loadTargetedCoursesWithWeights(
        const std::vector<uint16_t>& coursesIds) const
    {
        LOG((*logger_), "Loading courses targeted by all the requests");
        std::stringstream filter;
        filter << "Id IN (";
        for (const auto& id : coursesIds) { filter << std::to_string(id) << ","; }
        // Move write buffer one character back
        filter.seekp(-1, filter.cur);
        filter << ")";
        std::vector<Course> unorganizedCoruses = SqlAdapter_->getCourses(filter.str());
        std::map<uint16_t, courseWithWeights> fetchedCourses{};
        for (auto& course : unorganizedCoruses)
        {
            uint16_t id = course.id_;
            // Assign subject to course weight
            courseWithWeights pair = std::make_pair(course, std::vector<CourseSubjectWeight>());
            fetchedCourses.insert(std::make_pair(id, std::move(pair)));
        }
        return fetchedCourses;
    }

    // @TODO Finish this function
    std::map<uint16_t, studentWithGrades> RequestResolver::loadInvokingStudentsWithGrades(
        const std::vector<uint16_t>& studentIds) const
    {
        LOG((*logger_), "Loading students that invoked all the requests");
        std::stringstream filter;
        filter << "Id IN (";
        for (const auto& id : studentIds) { filter << std::to_string(id) << ","; }
        // Move write buffer one character back
        filter.seekp(-1, filter.cur);
        filter << ")";
        std::vector<Student> unorganizedEntries = SqlAdapter_->getStudents(filter.str());
        std::map<uint16_t, studentWithGrades> fetchedStudents{};
        for (auto& student : unorganizedEntries)
        {
            uint16_t id = student.id_;
            // Assign subject to course weight
            studentWithGrades pair = std::make_pair(student, std::vector<Grade>());
            fetchedStudents.insert(std::make_pair(id, std::move(pair)));
        }
        return fetchedStudents;
    }

    bool RequestResolver::handleRequest(studentWithGrades invoker, courseWithWeights target)
    {
        // Log here would be too verbose.
        Course targetCourse                           = target.first;
        std::vector<CourseSubjectWeight> targetWeigts = target.second;

        Student invokerStudent           = invoker.first;
        std::vector<Grade> invokerGrades = invoker.second;

        return true;
    }

    void RequestResolver::run()
    {
        LOG((*logger_), "Running request resolver.");
        if (pendingRequests_.empty())
        {
            LOG((*logger_), "No pending requests in DB");
            std::cout << "No pending requests found!\n";
            return;
        }

        std::map<uint16_t, courseWithWeights> loadedCourses  = loadTargetedCoursesWithWeights(extractCourseIds());
        std::map<uint16_t, studentWithGrades> loadedStudents = loadInvokingStudentsWithGrades(extractStudentIds());

        for (auto& request : pendingRequests_)
        {
            if (handleRequest(loadedStudents.at(request.studentId_), loadedCourses.at(request.courseId_)))
            {
                // Pending -> Accepted
                continue;
            }

            // Pending -> Rejected
        }
        LOG((*logger_), "RequestResolver finished its work.");
    }
}  // namespace Core
