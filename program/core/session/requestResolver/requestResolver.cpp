#include "requestResolver.hpp"

#include "../../../utilities/common/constants.hpp"

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
        std::string pendingRequestsOnly           = "requestStatus = \"Pending\"";
        std::vector<Request::Srequest> requestVec = sqlAdapter_->getSrequests(pendingRequestsOnly);
        pendingRequests_                          = {};
        for (Request::Srequest req : requestVec) { pendingRequests_.insert(std::make_pair(req.id_, req)); }
        LOG((*logger_), "Initialized request resolver with ", pendingRequests_.size(), " student's requests");
    }

    void RequestResolver::assignAttendees(std::map<uint16_t, Course>& courses, const std::map<uint16_t, Student>& students)
    {
        using Utilities::Common::Constants::AttendeeValuePosition;
        using Utilities::Sql::rawAttendee;
        LOG((*logger_), "Assigning attendees to courses.");
        for (auto& course : courses)
        {
            const uint16_t& courseId  = course.first;
            Attendees& attendanceList = attendees_.at(courseId);
            std::stringstream filter;
            filter << "CourseId = " << courseId;
            std::vector<rawAttendee> attendees = sqlAdapter_->getAttendees(filter.str());
            // StudentID | CourseID | Grade/Score
            std::for_each(attendees.begin(), attendees.end(), [&](const rawAttendee& entry) {
                uint16_t studentId                     = std::get<static_cast<uint8_t>(AttendeeValuePosition::studentId)>(entry);
                double studentScore                    = std::get<static_cast<uint8_t>(AttendeeValuePosition::points)>(entry);
                std::shared_ptr<Student> targetStudent = std::make_shared<Student>(students.at(studentId));
                attendanceList.insertAttendee(targetStudent, studentScore);
            });
            LOG((*logger_), "Finished handling course: ", courseId, " and got n-", attendanceList.size(), " attendees");
        }
    }

    void RequestResolver::loadGrades()
    {
        LOG((*logger_), "Extracting grades information.");
        std::vector<Grade> gradesRaw = sqlAdapter_->getGrades();
        for (auto& grade : gradesRaw)
        {
            std::pair<uint16_t, Grade> entry = std::make_pair(grade.subjectId_, grade);
            if (grades_.contains(grade.studentId_)) { grades_.at(grade.studentId_).insert(entry); }
            else { grades_.insert(std::make_pair(grade.studentId_, std::map<uint16_t, Grade>{ entry })); }
        }
    }

    void RequestResolver::loadPresentAttendees(std::map<uint16_t, Course>& courses, const std::map<uint16_t, Student>& students)
    {
        LOG((*logger_), "Loading present course attendees.");
        using Utilities::Common::Constants::AttendeeValuePosition;
        using Utilities::Sql::rawAttendee;

        std::vector<rawAttendee> presentAttendees = sqlAdapter_->getAttendees();
        std::for_each(presentAttendees.begin(), presentAttendees.end(), [&](rawAttendee entry) {
            uint16_t courseId, studentId;
            double points;
            studentId = std::get<static_cast<uint8_t>(AttendeeValuePosition::studentId)>(entry);
            courseId  = std::get<static_cast<uint8_t>(AttendeeValuePosition::courseId)>(entry);
            points    = std::get<static_cast<uint8_t>(AttendeeValuePosition::points)>(entry);

            if (!courses.contains(courseId))
            {
                LOG((*logger_), "Skipped course with ID ", courseId, " due to not having any requests.");
                return;
            }

            std::shared_ptr<Student> targetStudent;
            if (students.contains(studentId)) { targetStudent = std::make_shared<Student>(students.at(studentId)); }
            else
            {
                std::stringstream ss;
                ss << "ID = " << studentId;
                std::vector<Student> nonRequestingStudent = sqlAdapter_->getStudents(ss.str());
                // If this returns value other than 1 we throw an error
                if (nonRequestingStudent.size() != 1)
                {
                    std::stringstream errorMsg;
                    errorMsg << "Failed while handling student with ID " << studentId
                             << (nonRequestingStudent.size() > 1 ? " more than 1 student have the same ID "
                                                                 : " no student found with given ID.");
                    throw std::runtime_error(errorMsg.str());
                }
                targetStudent = std::make_shared<Student>(nonRequestingStudent.at(0));
            }
            attendees_.at(courseId).insertAttendee(targetStudent, points);
            // No need on assigning to student as we only need to know that information from one obj
        });
        LOG((*logger_), "Finished loading attendees.");
    }

    double RequestResolver::calculatePoints(const Student& invoker, Course& target)
    {
        double pointVerdict = 0;
        // Map student grades by its subjectId for later ease of access
        std::map<uint16_t, Grade> remappedGrades = grades_.at(invoker.id_);
        for (const auto& [weightId, weight] : target.subjectWithWeight_)
        {
            // Calculate
            uint16_t& subjectId = weight->subjectId_;
            double gradeValue   = 0.0f;

            Grade& grade         = remappedGrades.at(subjectId);
            gradeValue           = grade.value_;
            double weightVerdict = gradeValue * weight->weight_;
            pointVerdict += weightVerdict;
            LOG((*logger_), "Calculated points: subjectId=", subjectId, " weight=", weight->weight_, " value=", gradeValue);
        }
        LOG((*logger_), "Final verdict pointVerdict <=> acceptanceThershold: ", pointVerdict, " <=> ", target.baseMinimalPoints_);
        return pointVerdict;
    }

    std::unordered_set<uint16_t> RequestResolver::extractIds(std::function<uint16_t(Request::Srequest)> extractConcrete) const
    {
        LOG((*logger_), "Extracting IDs");
        std::unordered_set<uint16_t> extractedIds;
        for (const auto& request : pendingRequests_) { extractedIds.insert(extractConcrete(request.second)); }
        LOG((*logger_), "Got n=", extractedIds.size(), " ids.");
        return extractedIds;
    }

    void RequestResolver::addToCourse(Course& course, Student& student, double points, const uint16_t requestId)
    {
        LOG((*logger_), "Adding student ", student.email_, " to ", course.name_);
        Attendees& attendeesList = attendees_.at(course.id_);
        if (attendeesList.size() == course.maxStudents_)
        {
            uint16_t currentMinId   = attendeesList.getMinId();
            double currentMinPoints = attendeesList.getAttendeePoints(currentMinId);
            if (attendeesList.insertAttendee(std::make_shared<Student>(student), points) ==
                Attendees::InsertionStatus::addedMinimumChangedWithMaxCapacity)
            {
                // Current minimal Id has changed, need to modify the request status for previous student ID.
                LOG((*logger_), "Previous minimal ID: ", currentMinId, " - ", currentMinPoints, " pts, changed to ", student.id_,
                    " - ", points, " points.");
                std::pair<uint16_t, uint16_t> studentCourseIdPair = std::make_pair(student.id_, course.id_);
                acceptedStudents_.insert(std::make_pair(studentCourseIdPair, requestId));
                acceptedStudents_.erase(std::make_pair(currentMinId, course.id_));
                return;
            }
            // Could not add - less than current minimal student.
            return;
        }
        else if (attendeesList.size() < course.maxStudents_)
        {
            attendeesList.insertAttendee(std::make_shared<Student>(student), points);
            std::pair<uint16_t, uint16_t> studentCourseIdPair = std::make_pair(student.id_, course.id_);
            acceptedStudents_.insert(std::make_pair(studentCourseIdPair, requestId));
            return;
        }
        // There will never be a situation where size() > course.maxStudents_
        LOG((*logger_), "Course capacity reached its peak of ", course.maxStudents_);
    }

    void RequestResolver::run()
    {
        LOG((*logger_), "Running request resolver. ");
        std::cout << "Running request resolver.";
        if (pendingRequests_.empty())
        {
            LOG((*logger_), "No pending requests in DB");
            std::cout << "No pending requests found!\n";
            return;
        }
        std::unordered_set<uint16_t> courseIds  = extractIds([](Request::Srequest req) { return req.courseId_; });
        std::unordered_set<uint16_t> studentIds = extractIds([](Request::Srequest req) { return req.studentId_; });
        std::cout << "Loading courses entries...\n";
        std::map<uint16_t, Course> mappedCourses =
            loadEntriesFromIds<Course>(courseIds, [this](std::string filter) { return sqlAdapter_->getCourses(filter); });
        for (const auto& [courseId, courseObj] : mappedCourses)
        {
            attendees_.insert(std::make_pair(courseId, Attendees(courseObj.maxStudents_)));
        }

        std::cout << "Loading student entries...\n";
        std::map<uint16_t, Student> mappedStudents =
            loadEntriesFromIds<Student>(studentIds, [this](std::string filter) { return sqlAdapter_->getStudents(filter); });

        std::cout << "Loading additional information - course subject weight mapping...\n";
        loadAdditionalInfo<Course, CourseSubjectWeight>(
            mappedCourses, [this](std::string filter) { return sqlAdapter_->getCourseSubjectWeight(filter); },
            &Course::subjectWithWeight_);
        std::cout << "Loading additional information - attendees mapping...\n";
        loadPresentAttendees(mappedCourses, mappedStudents);
        std::cout << "Loading additional information - student grades...\n";
        loadGrades();

        std::cout << "Calculating requests...\n";
        for (auto& [requestId, request] : pendingRequests_)
        {
            Course& target   = mappedCourses.at(request.courseId_);
            Student& invoker = mappedStudents.at(request.studentId_);
            std::cout << "Handling request " << requestId << "\n";
            LOG((*logger_), "Handling request ", requestId, " studentId=", invoker.id_, " courseId=", target.id_);
            if (double points = calculatePoints(invoker, target); points >= target.baseMinimalPoints_)
            {
                LOG((*logger_), "StudentID=", invoker.id_, " passed threshold for CourseID=", target.id_);
                std::cout << "Student " << invoker.firstName_ << " " << invoker.lastName_ << " (" << invoker.email_ << ") "
                          << "passed the threshold to join " << target.name_ << " (" << target.id_ << ")\n";
                addToCourse(target, invoker, points, request.id_);
            }
            else
            {
                LOG((*logger_), "StudentID=", invoker.id_, " did not passed threshold for CourseID=", target.id_);
                std::cout << "Student " << invoker.firstName_ << " " << invoker.lastName_ << " (" << invoker.email_ << ") "
                          << "did not passed the threshold to join " << target.name_ << " (" << target.id_ << ")\n";
            }
        }
        std::cout << "Calculating done!\n";
        handleAccepted();
        handleDenied();
        LOG((*logger_), "RequestResolver finished its work.");
    }

    void RequestResolver::handleAccepted()
    {
        std::cout << "Handling accepted students.\n";
        LOG((*logger_), "Handling accepted students.");
        std::vector<uint16_t> acceptedIds;
        std::vector<std::tuple<const uint16_t&, const u_int16_t&, const double>> valuesToAdd;
        for (const auto& [studentWithCourse, requestId] : acceptedStudents_)
        {
            const uint16_t& studentId = studentWithCourse.first;
            const uint16_t& courseId  = studentWithCourse.second;
            const double& points      = attendees_.at(courseId).getAttendeePoints(studentId);
            LOG((*logger_), "Setting request: ", requestId, " as 'passed' (studentId: ", studentId, " courseId: ", courseId,
                "), points = ", points);
            Request::Srequest& tmp = pendingRequests_.at(requestId);
            tmp.status_            = Request::requestStatus::Approved;
            acceptedIds.push_back(requestId);
            valuesToAdd.push_back(std::tuple<const uint16_t&, const uint16_t&, const double>(studentId, courseId, points));
        }
        sqlAdapter_->addAttendees(valuesToAdd);
        sqlAdapter_->updateRequestStatus(acceptedIds, Request::requestStatus::Approved);
    }

    void RequestResolver::handleDenied()
    {
        std::cout << "Handling denied students.\n";
        LOG((*logger_), "Handling denied students.");
        std::vector<uint16_t> deniedIds;
        for (auto& [reqId, req] : pendingRequests_)
        {
            if (req.status_ == Request::requestStatus::Pending)
            {
                LOG((*logger_), "Setting request: ", reqId, " as 'denied' (studentId: ", req.studentId_,
                    " courseId: ", req.courseId_, ")");
                req.status_ = Request::requestStatus::Denied;
                deniedIds.push_back(reqId);
            }
        }
        sqlAdapter_->updateRequestStatus(deniedIds, Request::requestStatus::Denied);
    }
}  // namespace Core
