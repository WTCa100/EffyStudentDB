#pragma once

#include "../entry.hpp"
#include "../../utilities/common/constants.hpp"

using Utilities::Common::Constants::g_tableGrades;
namespace Core::Types
{
    struct Grade : public Entry
    {
        double value_;
        uint16_t studentId_;
        uint16_t subjectId_;
        std::string studentName_;
        std::string subjectName_;

        std::string toString() const override;
        std::unordered_map<std::string, std::string> userConstruct(bool makeFull = true) override;
        std::map<std::string, std::string> getAttrs() const override;
        Entry& operator=(const Entry& other) override;

        Grade() : Entry(0, g_tableGrades) {}
        Grade(uint16_t id, double value, uint16_t studentId, uint16_t subjectId, std::string studentName = "", std::string subjectName = "");
    };
    
} // namespace Core::Types
