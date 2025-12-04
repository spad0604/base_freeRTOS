#include <stdbool.h>
#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "app_config.h"
#include "core/message_queue.h"
#include "os_log/os_log.h"
#include "os_log/os_time.h"

typedef struct
{
    uint32_t sequence;
    uint64_t timestamp_us;
} TaskPayload;

static MessageQueue g_main_queue;

static void consumer_task(void *args);
static void producer_task(void *args);

void app_main(void)
{
    os_log_set_level(APP_CFG_LOG_LEVEL);

    if (!message_queue_init(&g_main_queue, "main_queue", APP_CFG_MESSAGE_QUEUE_DEPTH))
    {
        OS_LOGE("APP", "Failed to initialize message queue");
        return;
    }

    BaseType_t created = xTaskCreate(
        consumer_task,
        "task_main",
        APP_CFG_CONSUMER_TASK_STACK,
        NULL,
        APP_CFG_CONSUMER_TASK_PRIO,
        NULL);

    if (created != pdPASS)
    {
        OS_LOGE("APP", "Failed to create consumer task");
        return;
    }

    created = xTaskCreate(
        producer_task,
        "task_qr",
        APP_CFG_PRODUCER_TASK_STACK,
        NULL,
        APP_CFG_PRODUCER_TASK_PRIO,
        NULL);

    if (created != pdPASS)
    {
        OS_LOGE("APP", "Failed to create producer task");
    }
}

static void producer_task(void *args)
{
    (void)args;
    uint32_t counter = 0U;
    const TickType_t period_ticks = app_cfg_ms_to_ticks(APP_CFG_PRODUCER_PERIOD_MS);

    while (true)
    {
        TaskPayload *payload = pvPortMalloc(sizeof(*payload));
        if (payload == NULL)
        {
            OS_LOGW("QR", "Allocation failed, retrying");
            vTaskDelay(period_ticks);
            continue;
        }

        payload->sequence = ++counter;
        payload->timestamp_us = os_time_get_cpu_time_us();

        const Sender sender = sender_from_current_task();
        const TickType_t timeout = app_cfg_ms_to_ticks(APP_CFG_QUEUE_TX_TIMEOUT_MS);
        const bool sent = message_queue_send(&g_main_queue, sender, payload, timeout);

        if (!sent)
        {
            OS_LOGW("QR", "Queue full, dropping seq %lu", (unsigned long)payload->sequence);
            vPortFree(payload);
        }
        else
        {
            OS_LOGD("QR", "Sent seq %lu", (unsigned long)payload->sequence);
        }

        vTaskDelay(period_ticks);
    }
}

static void consumer_task(void *args)
{
    (void)args;
    Message message;

    while (true)
    {
        if (message_queue_receive(&g_main_queue, &message, portMAX_DELAY))
        {
            TaskPayload *payload = (TaskPayload *)message.data;
            const char *sender_name = (message.sender.name != NULL) ? message.sender.name : "unknown";

            if (payload != NULL)
            {
                const uint32_t latency_ms = os_time_elapsed_ms(payload->timestamp_us);
                OS_LOGI(
                    "MAIN",
                    "Received seq %lu from %s after %lu ms",
                    (unsigned long)payload->sequence,
                    sender_name,
                    (unsigned long)latency_ms);
                vPortFree(payload);
            }
            else
            {
                OS_LOGW("MAIN", "Received NULL payload from %s", sender_name);
            }
        }
    }
}
