#include <stdio.h>
#include <time.h>

#ifndef LOG_LEVEL
#define LOG_LEVEL INFO
#endif
/*
* Макрос для логирования сообщений с указанием времени, уровня, функции и строки.
* ERROR - красный
* INFO - зеленый
* DEBUG - голубой.
*/
#define LOG(level, message, ...) do { \
    if (level >= LOG_LEVEL) { \
        time_t now = time(NULL); \
        char *timestamp = ctime(&now); \
        timestamp[24] = '\0'; \
        const char* color = (level == ERROR) ? "\033[31m" : (level == DEBUG) ? "\033[36m" : "\033[32m"; \
        const char* lvl_str = (level == ERROR) ? "ERROR" : (level == DEBUG) ? "DEBUG" : "INFO"; \
        fprintf(stderr, "%s[%s] [%s] [%s:%d]: " message "\033[0m\n", \
                color, timestamp, lvl_str, __func__, __LINE__, ##__VA_ARGS__); \
    } \
} while(0)