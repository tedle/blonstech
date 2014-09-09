#include "loggeride.h"

LoggerIDE::LoggerIDE(Level log_level)
{
    log_level_ = log_level;
}

void LoggerIDE::out(std::string msg, Level log_level)
{
    if (log_level >= log_level_)
    {
        // msg += "\n";
        OutputDebugStringA(msg.c_str());
    }
}