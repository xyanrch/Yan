#include <cstdarg>
#include <cstdio>
#include <cstring>
#include "log.h"
#include "token.h"
namespace Yan
{
#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_BLUE "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN "\x1b[36m"
#define ANSI_COLOR_RESET "\x1b[0m"

  const char *program = "Yan";

  Loglevel log_level = L_DEBUG;
  static   std::string levelToString(Loglevel l)
  {
    switch(l)
    {
      case L_DEBUG: return "[DEBUG] ";
      case L_NOTICE: return "[NOTICE] ";
      case L_ERROR_EXIT: return "[ERROR] ";
    }
  };

  logger::logger(std::ostream &os, const char *func, int line, Loglevel level) : impl(os, func, line, level)
  {
  }
  logger::logger(std::ostream&os, Loglevel level):impl(os,level)
  {

  }
  logger::Impl::Impl(std::ostream &os, const char *func, int line, Loglevel level) : os_(os)
  {
    stream_ << ANSI_COLOR_GREEN<<levelToString(level)<<ANSI_COLOR_RESET<<func << ": line: " << line<<" " ;
    level_ = level;
  }

  logger::Impl::Impl(std::ostream& os, Loglevel level):os_(os)
  {
    stream_<<ANSI_COLOR_RED<<levelToString(level)<<ANSI_COLOR_RESET<<" ";
    level_ = level;
  }
  logger::Impl:: ~Impl() 
  { 
    os_ << stream_.str() << std::endl;

    if(level_ == L_ERROR_EXIT)
    {
      exit(-1);
    }
    
   }

  void static vError(const location &loc,
                     const char *format,
                     va_list args)
  {
    fprintf(stderr,
            "%s:%d:%d: " ANSI_COLOR_RED "error: " ANSI_COLOR_RESET,
            loc.fileName.c_str(),
            loc.line,
            loc.colum);
    vfprintf(stderr, format, args);
    fprintf(stderr, "\n    ");
    /*
  bool sawNoSpace = false;
  int nspaces = 0;
  for (auto p = loc.lineBegin_; *p != '\n' && *p != 0; p++) {
    if (!sawNoSpace && (*p == ' ' || *p == '\t')) {
      ++nspaces;
    } else {
      sawNoSpace = true;
      fputc(*p, stderr);
    }
  }

  fprintf(stderr, "\n    ");
  for (unsigned i = 1; i + nspaces < loc.column_; ++i)
    fputc(' ', stderr);*/
    fprintf(stderr, ANSI_COLOR_GREEN "^\n");
    exit(-1);
  }
  void ExitWithError(const char *format, ...)
  {
    fprintf(stderr,
            "%s: " ANSI_COLOR_RED "error: " ANSI_COLOR_RESET,
            program);

    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);

    fprintf(stderr, "\n");

    exit(1);
  }

  void Info(const char *format, ...)
  {
    fprintf(stderr,
            "%s: " ANSI_COLOR_GREEN "info: " ANSI_COLOR_RESET,
            program);

    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);

    fprintf(stderr, "\n");
  }

  void ExitWithError(const location &loc, const char *format, ...)
  {
    va_list args;
    va_start(args, format);
    vError(loc, format, args);
    va_end(args);
  }

} // namespace Yan