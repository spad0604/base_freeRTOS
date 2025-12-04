#include "os/os_log.h"

#include <stdio.h>
#include <stdarg.h>

#include "app_config.h"
#include "os_log/os_time.h"

#ifndef APP_CFG_LOG_LEVEL
#define APP_CFG_LOG_LEVEL OS_LOG_LEVEL_INFO
#endif

static os_log_level_t s_log_level = APP_CFG_LOG_LEVEL;

void os_log_set_level(os_log_level_t level)
{
    if (level < OS_LOG_LEVEL_NONE)
    {
        level = OS_LOG_LEVEL_NONE;
    }
    else if (level > OS_LOG_LEVEL_DEBUG)
    {
        level = OS_LOG_LEVEL_DEBUG;
    }

    s_log_level = level;
}

os_log_level_t os_log_get_level(void)
{
    return s_log_level;
}

static const char *level_to_label(os_log_level_t level)
{
    switch (level)
    {
        case OS_LOG_LEVEL_ERROR: return "E";
        case OS_LOG_LEVEL_WARN:  return "W";
        case OS_LOG_LEVEL_INFO:  return "I";
        case OS_LOG_LEVEL_DEBUG: return "D";
        default: return "-";
    }
}

void os_log_vprint(os_log_level_t level, const char *tag, const char *format, va_list args)
{
    if ((level == OS_LOG_LEVEL_NONE) || (level > s_log_level) || (format == NULL))
    {
        return;
    }

    uint32_t timestamp_ms = os_time_get_cpu_time_ms();
    const char *label = level_to_label(level);
    const char *safe_tag = (tag != NULL) ? tag : "LOG";

    printf("[%lu ms][%s][%s] ", (unsigned long)timestamp_ms, label, safe_tag);
    vprintf(format, args);
    printf("\r\n");
}

void os_log_print(os_log_level_t level, const char *tag, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    os_log_vprint(level, tag, format, args);
    va_end(args);
}
