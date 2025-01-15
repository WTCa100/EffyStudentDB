#pragma once

#include <string>
#include <map>

#include "../entry.hpp"
#include "../student/student.hpp"

namespace Core::Types
{
    struct School : public Entry
    {
        // SQL-related attribute members
        std::string name_;

        // Non-SQL-related attribute members
        std::map<uint16_t, Student> students_;

        School() : Entry("Schools"){}
        School(std::string name) : Entry("Schools"), name_(name), students_() {}
        School(uint16_t id, std::string name) : Entry(id, "Schools"), name_(name), students_() {}
        School(uint16_t id, std::string name, std::map<uint16_t, Student> studentList) : Entry(id, "Schools"), name_(name), students_(studentList) {}

        std::string toString() const override;
        virtual std::map<std::string, std::string> getAttrs() const override;
        void userConstruct(bool makeFull = true) override;

    };
} // namespace Core

