#pragma once

#include <map>
#include <string>
#include <optional>
#include <memory>

#include "../subject/subject.hpp"
#include "../entry.hpp"

namespace Core::Types
{
    struct Student : public Entry
    {
        std::string firstName_;
        std::optional<std::string> secondName_;
        std::string lastName_;
        std::string email_;
        std::map<std::string, float> grades_;
        uint16_t schoolId_;

        std::string toString(bool showGrades) const;
        std::string toString() const override;
        std::map<std::string, std::string> getAttrs() const override;
        void userConstruct(bool makeFull = true) override; 

        Student() : Entry(0, "Students") {}
        Student(uint16_t id, std::string name, std::string lastName, std::string email, uint16_t schoolId, std::optional<std::string> secondName = std::nullopt);
        Student(std::string name, std::string lastName, std::string email, uint16_t schoolId, std::optional<std::string> secondName = std::nullopt);
    };
} // namespace Core::Types
