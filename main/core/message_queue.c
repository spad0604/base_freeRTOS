#include "message_queue.h"

static bool queue_is_valid(const MessageQueue *queue)
{
    return (queue != NULL) && (queue->handle != NULL);
}

Sender sender_from_task(TaskHandle_t task, const char *name)
{
    Sender sender = {
        .task = task,
        .name = name ? name : (task ? pcTaskGetName(task) : NULL),
    };
    return sender;
}

Sender sender_from_current_task(void)
{
    TaskHandle_t current = xTaskGetCurrentTaskHandle();
    return sender_from_task(current, pcTaskGetName(current));
}

bool message_queue_init(MessageQueue *queue, const char *name, UBaseType_t depth)
{
    if ((queue == NULL) || (depth == 0))
    {
        return false;
    }

    queue->item_size = sizeof(Message);
    queue->depth = depth;
    queue->name = name;
    queue->handle = xQueueCreate(depth, queue->item_size);

    return queue->handle != NULL;
}

void message_queue_deinit(MessageQueue *queue)
{
    if (!queue_is_valid(queue))
    {
        return;
    }

    vQueueDelete(queue->handle);
    queue->handle = NULL;
    queue->depth = 0U;
    queue->item_size = 0U;
    queue->name = NULL;
}

bool message_queue_send(MessageQueue *queue, Sender sender, void *data, TickType_t timeout)
{
    if (!queue_is_valid(queue))
    {
        return false;
    }

    Message item = {
        .sender = sender,
        .data = data,
    };

    return xQueueSend(queue->handle, &item, timeout) == pdTRUE;
}

bool message_queue_receive(MessageQueue *queue, Message *out_message, TickType_t timeout)
{
    if (!queue_is_valid(queue) || (out_message == NULL))
    {
        return false;
    }

    return xQueueReceive(queue->handle, out_message, timeout) == pdTRUE;
}