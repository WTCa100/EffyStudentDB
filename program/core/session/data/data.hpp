#pragma once

#include <map>

#include "../../../types/entry.hpp"
#include "../../../types/school/school.hpp"
#include "../../../types/student/student.hpp"
#include "../../../types/subject/subject.hpp"
#include "../../../types/course/course.hpp"
#include "../../../types/studentRequest/request.hpp"

using Core::Types::School;
using Core::Types::Student;
using Core::Types::Subject;
using Core::Types::Course;
using Core::Types::Request::Srequest;
class SessionData
{
private:
    std::map<uint16_t, School> schoolList_;
    std::map<uint16_t, Student> studentList_;
    std::map<uint16_t, Subject> subjectList_;
    std::map<uint16_t, Course> courseList_;
    std::map<uint16_t, Srequest> sRequestsList_;

public:
    SessionData() : schoolList_({}), studentList_({}), subjectList_({}), courseList_({}), sRequestsList_({}) {}
    ~SessionData() = default;

    // @TODO: What if, all of these function were operating only on abstract entires? 
    // void addEntry(const Entry& newEntry)
    // In such function: Look for it's associated table
    // Each table shall have it's list assign to it

    void addSchool(const School& newSchool);
    void updateSchool(const uint16_t targetSchool, const School& alteredSchool);
    void removeSchool(const uint16_t targetSchool);
    bool existsSchool(const uint16_t targetSchool) const;

    void addStudent(const Student& newStudent);
    void removeStudent(const Student& targetStudent);

    void addSubject(const Subject& newSubject);
    void removeSubject(const Subject& targetSubject);

    void addGrade(const uint16_t targetSubject, const uint16_t targetStudent, float value);
    void removeGrade(const uint16_t targetSubject, const uint16_t targetStudent);

    void addCourse(const Course& newCourse);
    void updateCourse(const uint16_t targetCourse, const Course& alteredCourse);
    void removeCourse(const uint16_t targetCourse);

    void addStudentRequest(const Srequest& newSrequest);
    void removeStudentRequest(const uint16_t targetSReq);

    void showSchools() const;
    void showStudents() const;
    void showSubjects() const;
    void showGrades() const;
    void showCourses() const;
    void showStudentRequests(bool detailed = false) const;

    const std::map<uint16_t, School>& getSchools() const { return schoolList_; } 
    const std::map<uint16_t, Student>& getStudents() const { return studentList_; }
    const std::map<uint16_t, Subject>& getSubjects() const { return subjectList_; }
    const std::map<uint16_t, Course>& getCourses() const { return courseList_; }
    const std::map<uint16_t, Srequest>& getSrequest() const { return sRequestsList_; }
};
