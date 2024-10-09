#include <iostream>
#include "data.hpp"

void SessionData::addSchool(const School& newSchool)
{
    if(schoolList_.contains(newSchool.id_))
    {
        return;
    }
    schoolList_.insert(std::make_pair(newSchool.id_, newSchool));
}

void SessionData::removeSchool(const School& targetSchool)
{
    if(!schoolList_.contains(targetSchool.id_))
    {
        return;
    }
    schoolList_.erase(targetSchool.id_);
}

void SessionData::addStudent(const Student& newStudnet)
{
    if(studentList_.contains(newStudnet.id_))
    {
        return;
    }

    studentList_.insert(std::make_pair(newStudnet.id_, newStudnet));
    // Assign a student to a school
    if(schoolList_.contains(newStudnet.schoolId_))
    {
        schoolList_.at(newStudnet.schoolId_).students_.push_back(newStudnet);
    }
}

void SessionData::addSubject(const Subject& newSubject)
{
    if(subjectList_.contains(newSubject.id_))
    {
        return;
    }
    subjectList_.insert(std::make_pair(newSubject.id_, newSubject));
}

void SessionData::addGrade(const uint16_t targetSubject, const uint16_t targetStudent, float value)
{
    if(!subjectList_.contains(targetSubject))
    {
        return;
    }

    if(!studentList_.contains(targetStudent))
    {
        return;
    }

    if(studentList_.at(targetStudent).grades_.contains(subjectList_.at(targetSubject).name_))
    {
        return;
    }

    studentList_.at(targetStudent).grades_.insert(std::make_pair(subjectList_.at(targetSubject).name_, value));
}

void SessionData::showSchools() const
{
    std::cout << "Showing schools:\n";
    for(const auto& school : schoolList_)
    {
        std::cout << school.first << ". " << school.second.name_ << ": " << school.second.students_.size() << " students.\n";
    }
}

void SessionData::showStudents() const
{
    std::cout << "Showing students:\n";
    for(const auto& student : studentList_)
    {
        std::cout << student.first << ". ";
        Core::Types::showStudent(student.second);
        std::cout << "\n";
    }
}

void SessionData::showSubjects() const
{
    std::cout << "Showing subjects:\n";
    for(const auto& subject : subjectList_)
    {
        std::cout << subject.first << ". " << subject.second.name_ << "\n";
    }
}

void SessionData::showGrades() const
{
    std::cout << "Showing grades:\n";
    for(const auto& student : studentList_)
    {
        std::cout << student.first << ". ";
        Core::Types::showStudent(student.second, true);
        std::cout << "\n";
    }
}