#include <stdarg.h>
#include "Logger.h"
#include "printf.h"

inline void Logger::put_string(const char * strFnName)
{
    while( *strFnName )
    {
        _putchar( *strFnName++ ); 
    }
}

inline void Logger::put_preamble(const char * strFnName)
{
    _putchar('[');
    put_string(strFnName);
    _putchar(']');
    _putchar(' ');
}

void Logger::LogEnter(const char * strFnName)
{
    put_preamble( strFnName );
    put_string("starts.\n");
}

void Logger::LogExit(const char * strFnName)
{
    put_preamble( strFnName );
    put_string("ends.\n");
}

void Logger::LogMessage(const char * strFnName, const char * strMessage)
{
    put_preamble( strFnName );
    put_string( strMessage );
}

void Logger::LogFormat(const char * strFnName, const char * strFmt, ...)
{
    va_list args;
    put_preamble( strFnName );
    
    va_start(args, strFmt);
    printf_(strFmt, args);
    va_end(args);
    _putchar('\n');
}

