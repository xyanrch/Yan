#ifndef _LOG_H_
#define _LOG_H_
#include <sstream>
#include <iostream>
#include "token.h"
namespace Yan
{
    enum Loglevel
    {
        L_DEBUG,
        L_NOTICE,
        L_ERROR_EXIT
    };
    class logger
    {
    public:
        logger(std::ostream &os, const char *func, int line, Loglevel level);
        logger(std::ostream &os, Loglevel level);
        std::ostream &stream() { return impl.stream_; }

    private:
        struct Impl
        {
            Impl(std::ostream &os, const char *func, int line, Loglevel level);
            Impl(std::ostream &os, Loglevel level);
            ~Impl();
            std::ostream &os_;
            std::stringstream stream_;
            Loglevel level_;
        };
        Impl impl;
    };

    extern Loglevel log_level;
#define DEBUG_LOG             \
    if (log_level <= L_DEBUG) \
    logger(std::cout, __func__, __LINE__, L_DEBUG).stream()
#define NOTICE_LOG             \
    if (log_level <= L_NOTICE) \
    logger(std::cout, __func__, __LINE__, L_NOTICE).stream()
#define ERROR_EXIT \
    logger(std::cerr, L_ERROR_EXIT).stream()

#define DEBUG(format, ...)    \
    if (log_level <= L_DEBUG) \
    Info(format, ##__VA_ARGS__)

#define NOTICE(format, ...)    \
    if (log_level <= L_NOTICE) \
    Info(format, ##__VA_ARGS__)
    //
    //ERROR handle functions, when there is something error with input .c file, print msg, then exit
    void ExitWithError(const char *format, ...);

    void Info(const char *format, ...);

    void ExitWithError(const location &loc, const char *format, ...);

}; // namespace Yan
#endif