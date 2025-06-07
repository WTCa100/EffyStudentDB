#pragma once

#include <iostream>
#include <optional>

namespace Utilities::Command
{
	enum class targetType
	{
		file      = 0,
		directory = 1
	};

	class CommandHandler
	{
	  protected:
		std::string command_;

	  public:
		CommandHandler():
			command_("")
		{}

		virtual bool execute() = 0;
		static std::string assambleFullPath(
			std::string target, std::optional<std::string> subPath = std::nullopt, std::optional<std::string> cwd = std::nullopt);

		virtual ~CommandHandler() {}
	};
}  // namespace Utilities::Command