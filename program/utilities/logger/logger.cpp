#include <fstream>
#include <string>
#include <sstream>
#include "logger.hpp"

namespace Utilities
{
    Logger::Logger(std::string logPath)
    {
        fileName_ = "EffyStudentDb-" + getFileTimestamp(createCurrentTimestamp());
        if(!touch((std::filesystem::path(logPath)), fileName_))
        {
            std::cout << "Could not create a log file!\n";
        }
    };

    bool Logger::touch(std::filesystem::path path, std::string fileName)
    {
        std::cout << "Attempting to write " << path.c_str() + fileName + logExt << "\n";
        filePtr_.open(std::string(path.c_str() + fileName + logExt));
        if(filePtr_.good())
        {
            return true;
        }
        
        filePtr_.close();
        return false;
    }

    bool Logger::close()
    {
        LOG((*this), "Closing Log file");
        if(filePtr_.is_open())
        {
            filePtr_.close();
            if(!filePtr_.is_open())
            {
                std::cout << "Success.\n";
                return true;
                
            }
        }
        else
        {
            std::cout << "File already closed\n";
            return true;
        }

        std::cout << "Could not close file.\n";
        return false;
    }

    std::string Logger::getFileTimestamp(const std::tm& ts)
    {
        std::stringstream formatTs;
        formatTs << std::put_time(&ts, "%Y%m%d_%H%M%S");
        return formatTs.str();
    }

    std::string Logger::getLogEntryTimestamp(const std::tm& ts)
    {
        std::stringstream formatTs;
        formatTs << std::put_time(&ts, "[%Y.%m.%d - %H:%M:%S]");
        return formatTs.str();
    }

    Logger::~Logger()
    {
        std::cout << (close() ? "Logger closed successfully\n" : "Logger failed to close the logfile!\n");
    }
    
    const std::tm Logger::createCurrentTimestamp()
    {
        const auto now = std::chrono::system_clock::now();
        std::time_t fullDate = std::chrono::system_clock::to_time_t(now);
        std::tm timestamp = *std::localtime(&fullDate);
        return timestamp;
    } 
} // namespace Utilities

