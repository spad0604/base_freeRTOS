#include "os_time.h"

#include "esp_timer.h"

uint64_t os_time_get_cpu_time_us(void)
{
	return (uint64_t)esp_timer_get_time();
}

uint32_t os_time_get_cpu_time_ms(void)
{
	return (uint32_t)(os_time_get_cpu_time_us() / 1000ULL);
}

TickType_t os_time_get_tick_count(void)
{
	return xTaskGetTickCount();
}

uint32_t os_time_elapsed_ms(uint64_t start_us)
{
	uint64_t now = os_time_get_cpu_time_us();
	return (uint32_t)((now - start_us) / 1000ULL);
}
