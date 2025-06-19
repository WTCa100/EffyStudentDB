#pragma once

#include "../../utilities/common/constants.hpp"
#include "../entry.hpp"
#include "../student/student.hpp"

#include <map>
#include <string>

using Utilities::Common::Constants::g_tableSchools;

namespace Core::Types
{
    struct School : public Entry
    {
        // SQL-related attribute members
        std::string name_;

        // Non-SQL-related attribute members
        std::map<uint16_t, std::shared_ptr<Student>> students_;

        School():
            Entry(g_tableSchools),
            name_(""),
            students_({})
        {}

        School(std::string name):
            Entry(g_tableSchools),
            name_(name),
            students_()
        {}

        School(uint16_t id, std::string name):
            Entry(id, g_tableSchools),
            name_(name),
            students_()
        {}

        School(uint16_t id, std::string name, std::map<uint16_t, std::shared_ptr<Student>> studentList):
            Entry(id, g_tableSchools),
            name_(name),
            students_(studentList)
        {}

        std::string toString() const override;
        std::map<std::string, std::string> getAttrs() const override;
        std::unordered_map<std::string, std::string> userConstruct(bool makeFull = true) override;
        Entry& operator= (const Entry& other) override;
        std::shared_ptr<Entry> fillGaps(const std::shared_ptr<Entry> other) override;
    };
}  // namespace Core::Types

