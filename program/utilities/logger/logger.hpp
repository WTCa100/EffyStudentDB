#pragma once

#include <filesystem>
#include <fstream>
#include <iostream>
#include <optional>

#define LOG(logger, ...) logger.log(__FILE__, __LINE__, __VA_ARGS__);

namespace Utilities
{
	const std::string logExt = ".log";

	class Logger
	{
	  private:
		std::ofstream filePtr_;
		std::string fileName_;
		std::filesystem::path logPath_;

	  private:
		bool touch(std::filesystem::path path, std::string fileName);
		bool close();

	  public:
		Logger(std::string logPath = "./");

		const std::tm createCurrentTimestamp();
		std::string getFileTimestamp(const std::tm& ts);
		std::string getLogEntryTimestamp(const std::tm& ts);

		template<typename... Args> void log(const char* file, const int line, Args&&... args)
		{
			if (filePtr_.is_open())
			{
				filePtr_ << getLogEntryTimestamp(createCurrentTimestamp()) << ": " << file << ":" << std::to_string(line) << ": ";
				(filePtr_ << ... << args) << "\n";
			}
		}

		~Logger();
	};
}  // namespace Utilities

