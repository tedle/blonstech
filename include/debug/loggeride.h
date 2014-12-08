#ifndef BLONSTECH_DEBUG_LOGGERIDE_H_
#define BLONSTECH_DEBUG_LOGGERIDE_H_

//Local Includes
#include "logger.h"

namespace blons
{
class LoggerIDE : public LoggerAPI
{
public:
    LoggerIDE(Level log_level);

private:
    void out(std::string msg, Level log_level) override;
};
} // namespace blons

#endif