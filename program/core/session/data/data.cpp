#include <iostream>
#include "data.hpp"

void SessionData::addSchool(const School& newSchool)
{
    if(!schoolList_.contains(newSchool.id_))
    {
        schoolList_.insert(std::make_pair(newSchool.id_, newSchool));
    }
}

void SessionData::removeSchool(const uint16_t targetSchool)
{
    if(schoolList_.contains(targetSchool))
    {
        schoolList_.erase(targetSchool);
    }
}

void SessionData::addStudent(const Student& newStudnet)
{
    if(!studentList_.contains(newStudnet.id_))
    {
        studentList_.insert(std::make_pair(newStudnet.id_, newStudnet));
        // Assign a student to a school
        if(schoolList_.contains(newStudnet.schoolId_))
        {
            schoolList_.at(newStudnet.schoolId_).students_.push_back(newStudnet);
        }
    }
}

void SessionData::addSubject(const Subject& newSubject)
{
    if(!subjectList_.contains(newSubject.id_))
    {
        subjectList_.insert(std::make_pair(newSubject.id_, newSubject));
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

void SessionData::addCourse(const Course& newCourse)
{
    if(!courseList_.contains(newCourse.id_))
    {
        courseList_.insert(std::make_pair(newCourse.id_, newCourse));
    }
}

void SessionData::addStudentRequest(const Srequest& newSrequest)
{
    if(!sRequestsList_.contains(newSrequest.requestId_))
    {
        sRequestsList_.insert(std::make_pair(newSrequest.requestId_, newSrequest));
    }
}

void SessionData::showSchools() const
{
    std::cout << "Showing schools:\n";
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
                continue;
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