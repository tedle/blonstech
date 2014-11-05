#ifndef BLONSTECH_LOGGERIDE_H_
#define BLONSTECH_LOGGERIDE_H_

//Local Includes
#include "logger.h"

class LoggerIDE : public LoggerAPI
{
public:
    LoggerIDE(Level log_level);

private:
    void out(std::string msg, Level log_level);
};

#endif