#include <iostream>
#include "data.hpp"

void SessionData::addSchool(const School& newSchool)
{
    if(!schoolList_.contains(newSchool.id_))
    {
        schoolList_.insert(std::make_pair(newSchool.id_, newSchool));
    }
}

void SessionData::updateSchool(const uint16_t targetSchool, const School& alteredSchool)
{
    if(schoolList_.contains(targetSchool))
    {
        // Only thing we can change from the SQL is the name
        School& target = schoolList_.at(targetSchool);
        if(target.name_ != alteredSchool.name_)
        {
            target.name_ = alteredSchool.name_;
        }
    }
}

void SessionData::removeSchool(const uint16_t targetSchool)
{
    if(schoolList_.contains(targetSchool))
    {
        schoolList_.erase(targetSchool);
    }
}

bool SessionData::existsSchool(const uint16_t targetSchool) const
{
    return schoolList_.contains(targetSchool);
}

void SessionData::addStudent(const Student& newStudnet)
{
    if(!studentList_.contains(newStudnet.id_))
    {
        studentList_.insert(std::make_pair(newStudnet.id_, newStudnet));
        // Assign a student to a school
        std::cout << "DBG: Adding student " << newStudnet.schoolId_ << "\n";
        if(schoolList_.contains(newStudnet.schoolId_))
        {
            std::cout << "DBG: Have school adding it: " << schoolList_.at(newStudnet.schoolId_).name_ << "\n";
            schoolList_.at(newStudnet.schoolId_).students_.insert(std::make_pair(newStudnet.id_, newStudnet));
        }
    }
}

void SessionData::removeStudent(const Student& targetStudent)
{
    if(studentList_.contains(targetStudent.id_))
    {
        School& studentSchool = schoolList_.at(targetStudent.schoolId_);
        
        if(studentSchool.students_.contains(targetStudent.id_))
        {
            studentSchool.students_.erase(targetStudent.id_);
        }
        studentList_.erase(targetStudent.id_);
    }
}

void SessionData::addSubject(const Subject& newSubject)
{
    if(!subjectList_.contains(newSubject.id_))
    {
        subjectList_.insert(std::make_pair(newSubject.id_, newSubject));
    }
}

void SessionData::removeSubject(const Subject& targetSubject)
{
    if(subjectList_.contains(targetSubject.id_))
    {
        // Remove grades from all the students
        for(auto stud : studentList_)
        {
            if(stud.second.grades_.contains(targetSubject.name_))
            {
                stud.second.grades_.erase(targetSubject.name_);
            }
        }
        subjectList_.erase(targetSubject.id_);
    }
}

void SessionData::addGrade(const uint16_t targetSubject, const uint16_t targetStudent, float value)
{
    if(!subjectList_.contains(targetSubject) || !studentList_.contains(targetStudent))
    {
        return;
    }

    if(studentList_.at(targetStudent).grades_.contains(subjectList_.at(targetSubject).name_))
    {
        return;
    }

    studentList_.at(targetStudent).grades_.insert(std::make_pair(subjectList_.at(targetSubject).name_, value));
}

void SessionData::removeGrade(const uint16_t targetSubject, const uint16_t targetStudent)
{
    if(!subjectList_.contains(targetStudent) || !studentList_.contains(targetSubject))
    {
        return;
    }

    std::map<std::string, float>& targetGrades = studentList_.at(targetStudent).grades_;
    Subject& targetSub = subjectList_.at(targetSubject);
    if(targetGrades.contains(targetSub.name_))
    {
        targetGrades.erase(targetSub.name_);
    }
}

void SessionData::addCourse(const Course& newCourse)
{
    if(!courseList_.contains(newCourse.id_))
    {
        courseList_.insert(std::make_pair(newCourse.id_, newCourse));
    }
}

void SessionData::updateCourse(const uint16_t targetCourse, const Course& alteredCourse)
{
    if(courseList_.contains(targetCourse))
    {
        Course& target = courseList_.at(targetCourse);
        if(target.name_ != alteredCourse.name_)
        {
            target.name_ = alteredCourse.name_;
        }

        if(target.minStudents_ != alteredCourse.minStudents_)
        {
            target.minStudents_ = alteredCourse.minStudents_;
        }

        if(target.maxStudents_ != alteredCourse.maxStudents_)
        {
            target.maxStudents_ = alteredCourse.maxStudents_;
        }

        if(target.baseMinimalPoints_ != alteredCourse.baseMinimalPoints_)
        {
            target.baseMinimalPoints_ = alteredCourse.baseMinimalPoints_;
        }
    }
}

void SessionData::removeCourse(const uint16_t targetCourse)
{
    if(courseList_.contains(targetCourse))
    {
        courseList_.erase(targetCourse);
        for(auto& sr : sRequestsList_)
        {
            if(sr.second.courseId_)
            {
                // For now just deny every request - there may be a reason added later
                sr.second.status_ = Core::Types::Request::Denied;
            }
        }
    }
}

void SessionData::addStudentRequest(const Srequest& newSrequest)
{
    if(!sRequestsList_.contains(newSrequest.id_))
    {
        sRequestsList_.insert(std::make_pair(newSrequest.id_, newSrequest));
    }
}

void SessionData::removeStudentRequest(const uint16_t targetSReq)
{
    if(sRequestsList_.contains(targetSReq))
    {
        sRequestsList_.erase(targetSReq);
    }
}

void SessionData::showSchools() const
{
    std::cout << "Showing schools:\n";
    std::cout << "ID | SCHOOL NAME | STUDENT COUNT\n";
    for(const auto& school : schoolList_)
    {
        std::cout << school.second.toString() << "\n";
    }
}

void SessionData::showStudents() const
{
    std::cout << "Showing students:\n";
    for(const auto& student : studentList_)
    {
        std::cout << student.second.toString() << "\n";
    }
}

void SessionData::showSubjects() const
{
    std::cout << "Showing subjects:\n";
    for(const auto& subject : subjectList_)
    {
        std::cout << subject.second.toString() << "\n";
    }
}

void SessionData::showGrades() const
{
    std::cout << "Showing grades:\n";
    for(const auto& student : studentList_)
    {
        std::cout << student.second.toString(true) << "\n";
    }
}

void SessionData::showCourses() const
{
    std::cout << "Showing courses:\n";
    for(const auto& course : courseList_)
    {
        std::cout << course.second.toString() << "\n";
    }
}

void SessionData::showStudentRequests(bool detailed) const
{
    std::cout << "Showing student requests:\n";
    for(const auto& req : sRequestsList_)
    {
        std::cout << req.second.toString() << "\n";
        if(detailed)
        {
            if(studentList_.contains(req.second.studentId_))
            {
                const Student& stud = studentList_.at(req.second.studentId_);
                std::cout << "Student name & email: " << stud.firstName_ << " " << stud.lastName_ << " - " << stud.email_ << "\n";
            }
            else
            {
                std::cout << "Invalid student id - no entry matched in the memory!\n";
            }

            if(courseList_.contains(req.second.courseId_))
            {
                const Course& course = courseList_.at(req.second.courseId_);
                std::cout << "Course name: " << course.name_ << "\n";
            }
            else
            {
                std::cout << "Invalid course id - no entry matched in the memory!\n";
            }
        }
    }
}