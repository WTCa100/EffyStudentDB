#include "commandHandler.hpp"

namespace Utilities::Command
{
	std::string CommandHandler::assambleFullPath(
		std::string target, std::optional<std::string> subPath, std::optional<std::string> cwd)
	{
		std::string fullPath = "";
		if (cwd.has_value()) { fullPath += cwd.value() + "/"; }

		if (subPath.has_value()) { fullPath += subPath.value() + "/"; }

		fullPath += target;
		return fullPath;
	}
}  // namespace Utilities::Command
