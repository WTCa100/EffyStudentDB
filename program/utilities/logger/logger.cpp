#include <fstream>
#include <ctime>
#include <string>
#include <sstream>
#include "logger.hpp"

namespace Utilities
{
    Logger::Logger(std::string logPath)
    {
        fileName_ = createCurrentTimestamp();
        if(!touch((std::filesystem::path(logPath)), fileName_))
        {
            std::cout << "Could not create a log file!\n";
        }
        else
        {
            filePtr_ << ":" << __FILE__ << ":" << __LINE__ << ": Hello world!\n";
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
        std::cout << "Closing file " << fileName_ << "\n";
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

    Logger::~Logger()
    {
        std::cout << (close() ? "Logger closed successfully\n" : "Logger failed to close the logfile!\n");
    }
    
    const std::string Logger::createCurrentTimestamp()
    {
        const auto now = std::chrono::system_clock::now();
        std::time_t fullDate = std::chrono::system_clock::to_time_t(now);
        std::tm timestamp = *std::localtime(&fullDate);

        std::ostringstream oss;
        oss << std::put_time(&timestamp, "%Y%m%d_%H%M%S");
        return oss.str();
    } 


} // namespace Utilities

