#pragma once

#include <string>
#include <iostream>
#include <fstream>

namespace SparCraft
{

class Logger
{
        std::ofstream logStream;
        std::string logFile;

        Logger();

public:

        static Logger & Instance();
        void log(const std::string & msg);
        void setLogFile(const std::string & filename);
};
}
