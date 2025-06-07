#pragma once

#include "../../utilities/common/constants.hpp"
#include "../entry.hpp"
#include "../subject/subject.hpp"

#include <map>
#include <memory>
#include <optional>
#include <string>

using Utilities::Common::Constants::g_tableStudents;

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
		std::unordered_map<std::string, std::string> userConstruct(bool makeFull = true) override;

		Student():
			Entry(0, g_tableStudents)
		{}

		Student(uint16_t id,
			std::string name,
			std::string lastName,
			std::string email,
			uint16_t schoolId,
			std::optional<std::string> secondName = std::nullopt);
		Student(std::string name,
			std::string lastName,
			std::string email,
			uint16_t schoolId,
			std::optional<std::string> secondName = std::nullopt);
		Entry& operator= (const Entry& other) override;
	};
}  // namespace Core::Types
