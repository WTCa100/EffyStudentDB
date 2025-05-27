#pragma once

#include <string>
#include <map>

#include "../entry.hpp"
#include "../student/student.hpp"
#include "../../utilities/common/constants.hpp"

using Utilities::Common::Constants::g_tableSchools;

namespace Core::Types
{
    struct School : public Entry
    {
        // SQL-related attribute members
        std::string name_;

        // Non-SQL-related attribute members
        std::map<uint16_t, std::shared_ptr<Student>> students_;

        School() : Entry(g_tableSchools){}
        School(std::string name) : Entry(g_tableSchools), name_(name), students_() {}
        School(uint16_t id, std::string name) : Entry(id, g_tableSchools), name_(name), students_() {}
        School(uint16_t id, std::string name, std::map<uint16_t, std::shared_ptr<Student>> studentList) : Entry(id, g_tableSchools), name_(name), students_(studentList) {}

        std::string toString() const override;
        std::map<std::string, std::string> getAttrs() const override;
        std::unordered_map<std::string, std::string> userConstruct(bool makeFull = true) override;
        Entry& operator=(const Entry& other) override;

    };
} // namespace Core

