#include <stdio.h>
#include <time.h>
/*
* Уровни логирования
*/
typedef enum {
    INFO,
    DEBUG,
    ERROR
} LogLevel;
/*
* Макрос для логирования сообщений с указанием времени, уровня, функции и строки.
*/
#define LOG(level, message, ...) do { \
    time_t now = time(NULL); \
    char *timestamp = ctime(&now); \
    timestamp[24] = '\0'; \
    const char* lvl_str = (level == ERROR) ? "ERROR" : (level == DEBUG ? "DEBUG" : "INFO"); \
    fprintf(stderr, "[%s] [%s] [%s:%d]: " message "\n", \
            timestamp, lvl_str, __func__, __LINE__, ##__VA_ARGS__); \
} while(0)