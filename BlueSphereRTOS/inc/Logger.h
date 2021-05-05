#pragma once
//#define DISABLE_LOG_DEBUG

#define log_err(fmt, arg...) Logger::LogFormat(__func__, fmt, ##arg)

#ifndef DISABLE_LOG_DEBUG
#define log_enter()  Logger::LogEnter(__func__)
#define log_exit()  Logger::LogExit(__func__)
#define log_message(msg)  Logger::LogMessage(__func__, msg)
#define log_debug(fmt, arg...)  Logger::LogFormat(__func__, fmt, ##arg)
#else
#define log_enter()
#define log_exit()
#define log_message(msg)  
#define log_debug(fmt, arg...) 
#endif
	
class Logger
{
private:
    static inline void put_string(const char * strFnName);
    static inline void put_preamble(const char * strFnName);

public :
    ///<summary>Logs a function enter message in the form of "[function] starts.\n"</summary>
    ///<param name="strFnName">Function name. Use __func__ macro.</param>
    static void LogEnter(const char * strFnName);

    ///<summary>Logs a function exit message in the form of "[function] ends.\n"</summary>
    ///<param name="strFnName">Function name. Use __func__ macro.</param>
    static void LogExit(const char * strFnName);

    ///<summary>Logs a static message in the form of "[function] message.\n"</summary>
    ///<param name="strFnName">Function name. Use __func__ macro.</param>
    ///<param name="strMessage">User defined message</param>
    static void LogMessage(const char * strFnName, const char * strMessage);

    ///<summary>Logs a printf-formatted message in the form of "[function] formatted message\n"</summary>
    ///<param name="strFnName">Function name. Use __func__ macro.</param>
    ///<param name="strFmt">User defined message</param>
    static void LogFormat(const char * strFnName, const char * strFmt, ...);
};
