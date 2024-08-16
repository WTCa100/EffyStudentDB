#pragma once

#include <iostream>
#include <filesystem>
#include <fstream>

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
        const std::string createCurrentTimestamp(); 

        ~Logger();

    };
} // namespace Utilities

