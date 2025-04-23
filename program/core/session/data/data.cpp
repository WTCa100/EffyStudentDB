#include <iostream>
#include "data.hpp"

#include "../../../utilities/common/constants.hpp"

using namespace Utilities::Common::Constants;
SessionData::SessionData()
{
    // Initialize the entry list with empty maps for each table
    entryList_.insert(std::make_pair(g_tableSchools, std::map<uint16_t, std::shared_ptr<Entry>>()));
    entryList_.insert(std::make_pair(g_tableStudents, std::map<uint16_t, std::shared_ptr<Entry>>()));
    entryList_.insert(std::make_pair(g_tableSubjects, std::map<uint16_t, std::shared_ptr<Entry>>()));
    entryList_.insert(std::make_pair(g_tableCourses, std::map<uint16_t, std::shared_ptr<Entry>>()));
    entryList_.insert(std::make_pair(g_tableGrades, std::map<uint16_t, std::shared_ptr<Entry>>()));
    entryList_.insert(std::make_pair(g_tableStudentRequest, std::map<uint16_t, std::shared_ptr<Entry>>()));
    entryList_.insert(std::make_pair(g_tableCourseSubjectWeight, std::map<uint16_t, std::shared_ptr<Entry>>()));
}

std::set<std::string> SessionData::getTableNames() const
{
    std::set<std::string> names;
    for(const auto& table : entryList_)
    {
        names.insert(table.first);
    }
    return names;
}

bool SessionData::verifyTable(std::string tableName) const
{
    if(!entryList_.contains(tableName))
    {
        std::cout << "No table named: " << tableName << "\n";
        return false;
    }
    return true;
}

void SessionData::addEntry(const std::shared_ptr<Entry> newEntry)
{
    if(!verifyTable(newEntry->associatedTable_))
    {
        return;
    }

    std::map<uint16_t, std::shared_ptr<Entry>>& concreteMap = entryList_.at(newEntry->associatedTable_);
    if(!concreteMap.contains(newEntry->id_))
    {
        concreteMap.insert(std::make_pair(newEntry->id_, newEntry));
    }
}

void SessionData::removeEntry(const uint16_t targetId, const std::string& associatedTable)
{
    if(!verifyTable(associatedTable))
    {
        return;
    }
    std::map<uint16_t, std::shared_ptr<Entry>>& concreteMap = entryList_.at(associatedTable);
    if(concreteMap.contains(targetId))
    {
        concreteMap.erase(targetId);
    }
}

void SessionData::updateEntry(const uint16_t targetId, const std::shared_ptr<Entry> alteredEntry)
{
    if(!verifyTable(alteredEntry->associatedTable_))
    {
        return;
    }

    std::cout << "DBG: Target altered entry: " << alteredEntry.get()->toString() << "\n";
    concreteTypeList& concreteMap = entryList_.at(alteredEntry->associatedTable_);
    if(concreteMap.contains(targetId))
    {
        std::cout << "DBG: Before altering entry: " << concreteMap.at(targetId).get()->toString() << "\n";        
        *concreteMap.at(targetId) = *alteredEntry;
        std::cout << "DBG: Altered entry: " << concreteMap.at(targetId).get()->toString() << "\n";
    }
    else
    {
        std::cout << "!!! No such entry with id " << targetId << " in table " << alteredEntry->associatedTable_ << "\n";
    }
}

bool SessionData::isPresent(const uint16_t targetId, const std::string& associatedTable) const
{
    if(!verifyTable(associatedTable))
    {
        return false;
    }
    return entryList_.at(associatedTable).contains(targetId);
}

std::shared_ptr<Entry> SessionData::getEntry(const uint16_t targetId, const std::string& associatedTable)
{
    if(!verifyTable(associatedTable))
    {
        return nullptr;
    }
    
    if(entryList_.at(associatedTable).contains(targetId))
    {
        return entryList_.at(associatedTable).at(targetId);
    }

    return nullptr;
}

const std::unique_ptr<concreteTypeList> SessionData::getEntries(const std::string& table)
{
    if(!verifyTable(table))
    {
        std::cout << "No such table: " << table << "\n";
        return nullptr;
    }

    return std::make_unique<std::map<uint16_t, std::shared_ptr<Entry>>>(entryList_.at(table));
}

void SessionData::addGrade(const uint16_t targetSubject, const uint16_t targetStudent, float value)
{
    concreteTypeList studentList = entryList_.at(g_tableGrades);
    concreteTypeList subjectList = entryList_.at(g_tableStudents);

    if(!studentList.contains(targetStudent) || !subjectList.contains(targetSubject))
    {
        return;
    }

    std::shared_ptr<Student> concreteStudent = std::dynamic_pointer_cast<Student>(studentList.at(targetStudent));
    std::map<std::string, float>& concreteStudentGrades = concreteStudent->grades_; 
    std::string subjectName = std::dynamic_pointer_cast<Subject>(subjectList.at(targetSubject))->name_;
    if(concreteStudentGrades.contains(subjectName))
    {
        return;
    }
    concreteStudentGrades.insert(std::make_pair(subjectName, value));
}

void SessionData::removeGrade(const uint16_t targetSubject, const uint16_t targetStudent)
{

    concreteTypeList studentList = entryList_.at("Students");
    concreteTypeList subjectList = entryList_.at("Subjects");

    if(!studentList.contains(targetStudent) || !subjectList.contains(targetSubject))
    {
        return;
    }

    std::shared_ptr<Student> concreteStudent = std::dynamic_pointer_cast<Student>(studentList.at(targetStudent));
    std::map<std::string, float>& targetGrades = (*concreteStudent).grades_;
    std::shared_ptr<Subject> targetSub = std::dynamic_pointer_cast<Subject>(subjectList.at(targetSubject));
    if(targetGrades.contains(targetSub->name_))
    {
        targetGrades.erase(targetSub->name_);
    }
}