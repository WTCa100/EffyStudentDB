#include "requestResolver.hpp"

#include <sstream>

namespace Core
{
    RequestResolver::RequestResolver(
        std::shared_ptr<Logger> logger, std::shared_ptr<SqlAdapter> sqlAdapter, std::shared_ptr<SessionData> sesData):
        sesData_(sesData),
        logger_(logger),
        sqlAdapter_(sqlAdapter),
        pendingRequests_({})
    {
        std::string pendingRequestsOnly = "requestStatus = \"Pending\"";
        pendingRequests_                = sqlAdapter_->getSrequests(pendingRequestsOnly);
        LOG((*logger_), "Initialized request resolver with ", pendingRequests_.size(), " student's requests");
    }

    void RequestResolver::assignAttendees(std::map<uint16_t, Course>& courses, const std::map<uint16_t, Student>& students)
    {
        using Utilities::Sql::rawAttendee;
        using Utilities::Common::Constants::AttendeeValuePosition;
        LOG((*logger_), "Assigning attendees to courses.");
        for(auto& [courseId, courseInfo] : courses)
        {
            std::stringstream filter;
            filter << "CourseId = " << courseId; 
            std::vector<rawAttendee> attendees = sqlAdapter_->getAttendees(filter.str());
            // StudentID | CourseID | Grade/Score
            std::for_each(attendees.begin(), attendees.end(), [&](const rawAttendee& entry)
            { 
                uint16_t studentId = std::get<static_cast<uint8_t>(AttendeeValuePosition::studentId)>(entry);
                double studentScore = std::get<static_cast<uint8_t>(AttendeeValuePosition::points)>(entry);
                std::shared_ptr<Student> targetStudent = std::make_shared<Student>(students.at(studentId));
                courseInfo.attendees_.insert(std::make_pair(studentId, std::make_pair(targetStudent, studentScore)));
            });
            LOG((*logger_), "Finished handling course: ", courseId, " and got n-", courseInfo.attendees_.size(), " attendees");
        }
    }

    float RequestResolver::calculatePoints(const Student& invoker, Course& target)
    {
        float pointVerdict = 0;
        // Map student grades by its subjectId for later ease of access
        std::map<uint16_t, const std::shared_ptr<Grade>> remappedGrades;
        for (auto& grade : invoker.grades_) { remappedGrades.insert(std::make_pair(grade.second->subjectId_, grade.second)); }

        for (const auto& [weightId, weight] : target.subjectWithWeight_)
        {
            // Calculate
            uint16_t& subjectId = weight->subjectId_;
            float gradeValue    = 0.0f;
            if (remappedGrades.contains(subjectId))
            {
                std::shared_ptr<Grade> grade = remappedGrades.at(subjectId);
                gradeValue                   = grade->value_;
            }
            float weightVerdict = gradeValue * weight->weight_;
            pointVerdict += weightVerdict;
            LOG((*logger_), "Calculated points: subjectId=", subjectId, " weight=", weight->weight_, " value=", gradeValue);
        }
        LOG((*logger_), "Final verdict pointVerdict <=> acceptanceThershold: ", pointVerdict, " <=> ", target.baseMinimalPoints_);
        return pointVerdict >= target.baseMinimalPoints_;
    }

    std::unordered_set<uint16_t> RequestResolver::extractIds(std::function<uint16_t(Request::Srequest)> extractConcrete) const
    {
        LOG((*logger_), "Extracting IDs");
        std::unordered_set<uint16_t> extractedIds;
        for (const auto& request : pendingRequests_) { extractedIds.insert(extractConcrete(request)); }
        LOG((*logger_), "Got n=", extractedIds.size(), " ids.");
        return extractedIds;
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
        std::unordered_set<uint16_t> courseIds  = extractIds([](Request::Srequest req) { return req.courseId_; });
        std::unordered_set<uint16_t> studentIds = extractIds([](Request::Srequest req) { return req.studentId_; });
        std::map<uint16_t, Course> mappedCourses =
            loadEntriesFromIds<Course>(courseIds, [this](std::string filter) { return sqlAdapter_->getCourses(filter); });
        std::map<uint16_t, Student> mappedStudents =
            loadEntriesFromIds<Student>(studentIds, [this](std::string filter) { return sqlAdapter_->getStudents(filter); });

        loadAdditionalInfo<Course, CourseSubjectWeight>(
            mappedCourses, [this](std::string filter) { return sqlAdapter_->getCourseSubjectWeight(filter); },
            &Course::subjectWithWeight_);
        // TODO: Add assigniees.
        loadAdditionalInfo<Student, Grade>(
            mappedStudents, [this](std::string filter) { return sqlAdapter_->getGrades(filter); }, &Student::grades_);

        for (auto& request : pendingRequests_)
        {
            Course& target   = mappedCourses.at(request.courseId_);
            Student& invoker = mappedStudents.at(request.studentId_);
            // TODO Handle full course checking/handling + closing - for now just stick to adding
            std::cout << "Handling request " << request.id_ << "\n";
            LOG((*logger_), "Handling request ", request.id_, " studenId=", invoker.id_, " courseId=", target.id_);
            if (calculatePoints(invoker, target))
            {
                LOG((*logger_), "StudentID=", invoker.id_, " passed threshold for CourseID=", target.id_);
                std::cout << "Student " << invoker.firstName_ << " " << invoker.lastName_ << " (" << invoker.email_ << ") "
                          << "passed the thershold to join " << target.name_ << " (" << target.id_ << ")\b";
            }
            else
            {
                LOG((*logger_), "StudentID=", invoker.id_, " did not passed threshold for CourseID=", target.id_);
                std::cout << "Student " << invoker.firstName_ << " " << invoker.lastName_ << " (" << invoker.email_ << ") "
                          << "did not passed the thershold to join " << target.name_ << " (" << target.id_ << ")\n";
            }
        }

        LOG((*logger_), "RequestResolver finished its work.");
    }
}  // namespace Core
