#include "Logger.h"

using namespace SparCraft;

// constructor
Logger::Logger()
{
        logFile = "C:\\uab_log.txt";
        logStream.open(logFile.c_str(), std::ofstream::app);
}

// get an instance of this
Logger & Logger::Instance()
{
        static Logger instance;
        return instance;
}

void Logger::setLogFile(const std::string & filename)
{
        logFile = filename;
}

void Logger::log(const std::string & msg)
{
        logStream << msg;
    logStream.flush();
}
