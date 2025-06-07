#include "verifyCommand.hpp"

namespace Utilities::Command
{
	bool verifyCommand::execute()
	{
		std::string fullPath = assambleFullPath(target_, subPath_, std::filesystem::current_path().string());
		return std::filesystem::exists(fullPath);
	}

}  // namespace Utilities::Command
