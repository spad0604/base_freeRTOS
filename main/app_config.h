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

/* Networking / connectivity */
#ifndef APP_CFG_WIFI_SSID
#define APP_CFG_WIFI_SSID "your_ssid"
#endif

#ifndef APP_CFG_WIFI_PASSWORD
#define APP_CFG_WIFI_PASSWORD "your_password"
#endif

#if defined(APP_CFG_WIFI_OFF)
#define APP_CFG_WIFI_ENABLED 0
#else
#define APP_CFG_WIFI_ENABLED 1
#endif

/* Bluetooth (BLE) */
#ifndef APP_CFG_BLE_DEVICE_NAME
#define APP_CFG_BLE_DEVICE_NAME "fibonacci-ble"
#endif

#ifndef APP_CFG_BLE_ADV_INT_MIN_MS
#define APP_CFG_BLE_ADV_INT_MIN_MS 200U
#endif

#ifndef APP_CFG_BLE_ADV_INT_MAX_MS
#define APP_CFG_BLE_ADV_INT_MAX_MS 400U
#endif

#if defined(APP_CFG_BLUETOOTH_OFF)
#define APP_CFG_BLUETOOTH_ENABLED 0
#else
#define APP_CFG_BLUETOOTH_ENABLED 1
#endif

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "os/os_log.h"

static inline TickType_t app_cfg_ms_to_ticks(uint32_t ms)
{
    return pdMS_TO_TICKS(ms);
}

#endif // APP_CONFIG_H
