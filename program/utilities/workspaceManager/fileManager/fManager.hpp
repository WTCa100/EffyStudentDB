#pragma once

#include "../commandHandler/commandHandler.hpp"

#include <filesystem>
#include <fstream>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace Utilities::Workspace
{
	using Utilities::Command::CommandHandler;

	class FileManager
	{
	  private:
		std::unique_ptr<CommandHandler> comHandler_;
		const std::filesystem::path appRoot_;
		std::filesystem::path cwd_;

		void setCommandHandler(std::unique_ptr<CommandHandler> newCommandHandler) { comHandler_ = std::move(newCommandHandler); }

	  public:
		FileManager(std::string rootPath);
		FileManager();
		~FileManager() = default;

		std::filesystem::path getAppRoot() const { return appRoot_; }

		bool createFile(std::string fileName, std::optional<std::filesystem::path> subPath = std::nullopt);
		bool deleteFile(std::string fileName, std::optional<std::filesystem::path> subPath = std::nullopt);
		std::unique_ptr<std::fstream> getFile(std::string fileName, std::optional<std::filesystem::path> subPath = std::nullopt);
		std::vector<std::string> getFileContent(
			std::string fileName, std::optional<std::filesystem::path> subPath = std::nullopt);
		bool exist(std::string name, std::optional<std::filesystem::path> subPath = std::nullopt);
	};

}  // namespace Utilities::Workspace