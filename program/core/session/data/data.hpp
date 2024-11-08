#pragma once

#include <map>

#include "../../../types/school/school.hpp"
#include "../../../types/student/student.hpp"
#include "../../../types/subject/subject.hpp"
#include "../../../types/course/course.hpp"

using Core::Types::School;
using Core::Types::Student;
using Core::Types::Subject;
using Core::Types::Course;

class SessionData
{
private:
    std::map<uint16_t, School> schoolList_;
    std::map<uint16_t, Student> studentList_;
    std::map<uint16_t, Subject> subjectList_;
    std::map<uint16_t, Course> courseList_;

public:
    SessionData() : schoolList_({}), studentList_({}), subjectList_({}) {}
    ~SessionData() = default;

    void addSchool(const School& newSchool);
    void removeSchool(const School& targetSchool);

    void addStudent(const Student& newStudent);
    void addSubject(const Subject& newSubject);
    void addGrade(const uint16_t targetSubject, const uint16_t targetStudent, float value);

    void showSchools() const;
    void showStudents() const;
    void showSubjects() const;
    void showGrades() const;
};
