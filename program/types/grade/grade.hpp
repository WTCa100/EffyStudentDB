#pragma once

#include "../../utilities/common/constants.hpp"
#include "../entry.hpp"

using Utilities::Common::Constants::g_inputMissingValue;
using Utilities::Common::Constants::g_tableGrades;

namespace Core::Types
{
    struct Grade : public Entry
    {
        // SQL attribute members
        double value_;
        uint16_t studentId_;
        uint16_t subjectId_;

        // Non-SQL attribute members
        std::string studentName_;
        std::string subjectName_;

        std::string toString() const override;
        std::unordered_map<std::string, std::string> userConstruct(bool makeFull = true) override;
        std::map<std::string, std::string> getAttrs() const override;
        Entry& operator= (const Entry& other) override;
        std::shared_ptr<Entry> fillGaps(const std::shared_ptr<Entry> other) override;

        Grade():
            Entry(0, g_tableGrades),
            value_(0.0f),
            studentId_(0),
            subjectId_(0),
            studentName_(""),
            subjectName_("")
        {}

        Grade(uint16_t id,
            double value,
            uint16_t studentId,
            uint16_t subjectId,
            std::string studentName = "",
            std::string subjectName = "");
    };

}  // namespace Core::Types
