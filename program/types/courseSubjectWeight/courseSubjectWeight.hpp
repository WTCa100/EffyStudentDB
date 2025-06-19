#pragma once
#include "../../utilities/common/constants.hpp"
#include "../entry.hpp"

#include <string>

using Utilities::Common::Constants::g_tableCourseSubjectWeight;

namespace Core::Types
{
    /**
     * This struct represent a signle mapping of subject to course
     */
    struct CourseSubjectWeight : public Entry
    {
        double weight_;
        uint16_t courseId_;
        uint16_t subjectId_;
        std::string courseName_;
        std::string subjectName_;

        std::string toString() const override;
        std::unordered_map<std::string, std::string> userConstruct(bool makeFull = true) override;
        std::map<std::string, std::string> getAttrs() const override;
        Entry& operator= (const Entry& other) override;
        std::shared_ptr<Entry> fillGaps(const std::shared_ptr<Entry> other) override;

        CourseSubjectWeight();
        CourseSubjectWeight(uint16_t id,
            double weight,
            uint16_t courseId,
            uint16_t subjectId,
            std::string courseName  = "",
            std::string subjectName = "");
    };

}  // namespace Core::Types

