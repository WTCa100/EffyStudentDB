#pragma once

#include <map>

#include "../../../types/school/school.hpp"
#include "../../../types/student/student.hpp"
#include "../../../types/subject/subject.hpp"

using Core::Types::School;
using Core::Types::Student;
using Core::Types::Subject;

class SessionData
{
private:
    std::map<uint16_t, School> schoolList_;
    std::map<uint16_t, Student> studentList_;
    std::map<uint16_t, Subject> subjectList_;

public:
    SessionData() : schoolList_({}), studentList_({}), subjectList_({}) {}
    ~SessionData() = default;

    void addSchool(const School& newSchool);
    void addStudent(const Student& newStudent);
    void addSubject(const Subject& newSubject);
    void addGrade(const uint16_t targetSubject, const uint16_t targetStudent, float value);

    void showSchools() const;
    void showStudents() const;
    void showSubjects() const;
    void showGrades() const;
};
