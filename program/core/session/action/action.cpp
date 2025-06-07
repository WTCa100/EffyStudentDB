#include "action.hpp"

#include <iostream>

namespace Core
{
	std::string Action::getCommand() const
	{
		if (!tokens_.empty()) { return tokens_.at(static_cast<uint8_t>(ActionValue::command)); }

		std::cout << "Action::size() == 0\n";
		return "";
	}

	std::string Action::getTarget() const
	{
		if (tokens_.size() >= 2) { return tokens_.at(static_cast<uint8_t>(ActionValue::target)); }

		std::cout << "Action::size() < 2\n";
		return "";
	}

	std::vector<std::string> Action::getOptValues() const
	{
		if (tokens_.size() >= 3)
		{
			std::vector<std::string> tags =
				std::vector<std::string>(tokens_.begin() + static_cast<uint8_t>(ActionValue::optionalValues), tokens_.end());
			return tags;
		}

		std::cout << "Action::size < 3\n";
		return {};
	}

}  // namespace Core
