#pragma once

#include "../../utilities/common/constants.hpp"
#include "../entry.hpp"

#include <inttypes.h>
#include <string>
#include <vector>

using Utilities::Common::Constants::g_tableCourses;

namespace Core::Types
{
	struct Course : public Entry
	{
		std::vector<std::pair<uint16_t, uint16_t>> subjectWithWeight_;
		std::string name_;
		uint16_t minStudents_;
		uint16_t maxStudents_;
		uint16_t baseMinimalPoints_;
		uint16_t averageStudentPoints_;
		// Formula - on how to calculate points
		// One subject for now, range later (so, more than one subject can be applied to a given course)
		// Formula: [(1st subject grade * weight) + (2nd subject grade * weight) + (N-th subject grade * weight)] / N
		// Example: [(2 * math grade) + (1 * physics grade)] / 2

		std::string toString() const override;
		std::map<std::string, std::string> getAttrs() const override;
		std::unordered_map<std::string, std::string> userConstruct(bool makeFull = true) override;

		Course():
			Entry(0, g_tableCourses)
		{}

		Course(uint16_t id, uint16_t minStudents, uint16_t maxStudents, uint16_t baseMinimalPoints, std::string name);
		Course(uint16_t minStudents, uint16_t maxStudents, uint16_t baseMinimalPoints, std::string name);
		Entry& operator= (const Entry& other) override;
	};
}  // namespace Core::Types

