#pragma once

#include "../../../types/course/course.hpp"
#include "../../../types/entry.hpp"
#include "../../../types/school/school.hpp"
#include "../../../types/student/student.hpp"
#include "../../../types/studentRequest/request.hpp"
#include "../../../types/subject/subject.hpp"

#include <map>
#include <set>

using namespace Core::Types;
using namespace Core::Types::Request;
using abstractTypeList = std::map<uint16_t, std::shared_ptr<Entry>>;

class SessionData
{
  private:
    std::map<std::string, abstractTypeList> entryList_;
    // @TODO consider inserting maps from types to here
    // Like Students inside schools
    // std::map<uint16_t, std::set<uint16_t>> schoolsWithStudents_;
    // Each School can appear only once as well as each student ID can appear only exactly once inside a given entry.
    bool verifyTable(std::string tableName) const;

  public:
    SessionData();
    ~SessionData() = default;

    std::set<std::string> getTableNames() const;
    void addEntry(const std::shared_ptr<Entry> newEntry);
    void removeEntry(const uint16_t targetId, const std::string& associatedTable);
    void updateEntry(const uint16_t targetId, const std::shared_ptr<Entry> alteredEntry);
    bool isPresent(const uint16_t targetId, const std::string& associatedTable) const;
    std::shared_ptr<Entry> getEntry(const uint16_t targetId, const std::string& associatedTable);
    std::shared_ptr<abstractTypeList> getEntries(const std::string& table);
    void clearEntries();
};
