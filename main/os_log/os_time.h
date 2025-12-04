#ifndef OS_TIME_H
#define OS_TIME_H

#include <stdint.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Get monotonically increasing CPU time since boot in microseconds.
 */
uint64_t os_time_get_cpu_time_us(void);

/**
 * @brief Get CPU time since boot in milliseconds.
 */
uint32_t os_time_get_cpu_time_ms(void);

/**
 * @brief Get current FreeRTOS tick count.
 */
TickType_t os_time_get_tick_count(void);

/**
 * @brief Compute elapsed milliseconds between a captured timestamp and now.
 * @param start_us Timestamp captured via os_time_get_cpu_time_us.
 */
uint32_t os_time_elapsed_ms(uint64_t start_us);

#ifdef __cplusplus
}
#endif

#endif // OS_TIME_H
