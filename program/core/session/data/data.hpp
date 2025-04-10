#pragma once

#include <map>

#include "../../../types/entry.hpp"
#include "../../../types/school/school.hpp"
#include "../../../types/student/student.hpp"
#include "../../../types/subject/subject.hpp"
#include "../../../types/course/course.hpp"
#include "../../../types/studentRequest/request.hpp"

using namespace Core::Types;
using namespace Core::Types::Request;
using concreteTypeList = std::map<uint16_t, std::shared_ptr<Entry>>;
class SessionData
{
private:

    std::map<std::string, concreteTypeList> entryList_;
    bool verifyTable(std::string tableName) const;

public:
    // SessionData() : schoolList_({}), studentList_({}), subjectList_({}), courseList_({}), sRequestsList_({}) {}
    SessionData();

    ~SessionData() = default;

    void addEntry(const std::shared_ptr<Entry> newEntry);

    void removeEntry(const uint16_t targetId, const std::string& associatedTable);
    void updateEntry(const uint16_t targetId, const std::shared_ptr<Entry> alteredEntry);
    bool isPresent(const uint16_t targetId, const std::string& associatedTable) const;
    std::shared_ptr<Entry> getEntry(const uint16_t targetId, const std::string& associatedTable);
    const std::unique_ptr<concreteTypeList> getEntries(const std::string& table);

    void addGrade(const uint16_t targetSubject, const uint16_t targetStudent, float value);
    void removeGrade(const uint16_t targetSubject, const uint16_t targetStudent);
};
