#ifndef APP_CONFIG_H
#define APP_CONFIG_H

#include <stdint.h>

/* Uncomment to mute every log (compile-time switch) */
/* #define APP_CFG_LOG_DEBUG_OFF */

#if !defined(APP_CFG_LOG_LEVEL)
    #if defined(APP_CFG_LOG_DEBUG_OFF)
        #define APP_CFG_LOG_LEVEL OS_LOG_LEVEL_NONE
    #else
        #define APP_CFG_LOG_LEVEL OS_LOG_LEVEL_DEBUG
    #endif
#endif

#define APP_CFG_MESSAGE_QUEUE_DEPTH  8U
#define APP_CFG_QUEUE_TX_TIMEOUT_MS   50U
#define APP_CFG_PRODUCER_PERIOD_MS  1000U
#define APP_CFG_CONSUMER_TASK_STACK  4096
#define APP_CFG_PRODUCER_TASK_STACK  4096
#define APP_CFG_CONSUMER_TASK_PRIO  (tskIDLE_PRIORITY + 3)
#define APP_CFG_PRODUCER_TASK_PRIO  (tskIDLE_PRIORITY + 2)

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "os_log/os_log.h"

static inline TickType_t app_cfg_ms_to_ticks(uint32_t ms)
{
    return pdMS_TO_TICKS(ms);
}

#endif // APP_CONFIG_H
