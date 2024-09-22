#pragma once

#include <map>

#include "../../../types/school/school.hpp"
#include "../../../types/student/student.hpp"
#include "../../../types/subject/subject.hpp"

using Core::Types::School;
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
    void addGrade(const Subject& targetSub, const Student& targetStudent, float value);
};
