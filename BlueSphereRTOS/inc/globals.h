//#define DISABLE_LOG_DEBUG

#define log_err(fmt, arg...)\
	do {\
		printf_(__func__);\
        _putchar(':');\
		printf_(fmt, ##arg);\
        _putchar('\r');\
	} while (0)

#ifndef DISABLE_LOG_DEBUG
#define log_debug(fmt, arg...)\
	do {\
		printf_(__func__);\
        _putchar(':');\
		printf_(fmt, ##arg);\
        _putchar('\r');\
	} while (0)
#else
#define log_debug(fmt, arg...)
#endif