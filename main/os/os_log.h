#ifndef OS_LOG_H
#define OS_LOG_H

#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    OS_LOG_LEVEL_NONE = 0,
    OS_LOG_LEVEL_ERROR,
    OS_LOG_LEVEL_WARN,
    OS_LOG_LEVEL_INFO,
    OS_LOG_LEVEL_DEBUG,
} os_log_level_t;

void os_log_set_level(os_log_level_t level);
os_log_level_t os_log_get_level(void);

void os_log_print(os_log_level_t level, const char *tag, const char *format, ...);
void os_log_vprint(os_log_level_t level, const char *tag, const char *format, va_list args);

#if defined(APP_CFG_LOG_DEBUG_OFF)
    #define OS_LOGE(tag, fmt, ...) ((void)0)
    #define OS_LOGW(tag, fmt, ...) ((void)0)
    #define OS_LOGI(tag, fmt, ...) ((void)0)
    #define OS_LOGD(tag, fmt, ...) ((void)0)
#else
    #define OS_LOGE(tag, fmt, ...) os_log_print(OS_LOG_LEVEL_ERROR, tag, fmt, ##__VA_ARGS__)
    #define OS_LOGW(tag, fmt, ...) os_log_print(OS_LOG_LEVEL_WARN,  tag, fmt, ##__VA_ARGS__)
    #define OS_LOGI(tag, fmt, ...) os_log_print(OS_LOG_LEVEL_INFO,  tag, fmt, ##__VA_ARGS__)
    #define OS_LOGD(tag, fmt, ...) os_log_print(OS_LOG_LEVEL_DEBUG, tag, fmt, ##__VA_ARGS__)
#endif

#ifdef __cplusplus
}
#endif

#endif // OS_LOG_H