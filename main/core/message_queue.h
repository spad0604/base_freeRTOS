#ifndef MESSAGE_QUEUE_H
#define MESSAGE_QUEUE_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    TaskHandle_t task;      //< Task handle used as sender identifier
    const char *name;       //< Optional name for debug output
} Sender;

typedef struct
{
    Sender sender;          //< Producer metadata
    void *data;             //< Arbitrary payload pointer managed by caller
} Message;

typedef struct
{
    QueueHandle_t handle;   //< Underlying FreeRTOS queue
    UBaseType_t depth;      //< Number of items the queue can store
    size_t item_size;       //< Cached item size (sizeof(Message))
    const char *name;       //< Used for logging/diagnostics
} MessageQueue;

/**
 * @brief Create a Sender structure from a given task handle and optional name.
 * If name is NULL, the task's name will be used if available.
 * @param task The task handle to associate with the sender.
 * @param name Optional name for the sender; if NULL, the task's name is used
 */
Sender sender_from_task(TaskHandle_t task, const char *name);

/**
 * @brief Create a Sender structure for the currently running task.
 * The task's name will be used as the sender's name.
 */
Sender sender_from_current_task(void);

/**
 * @brief Initialize a MessageQueue with the specified name and depth.
 * @param queue Pointer to the MessageQueue to initialize.
 * @param name Name for the queue (used for diagnostics).
 * @param depth Maximum number of messages the queue can hold.
 * @return true if initialization was successful, false otherwise.
 */
bool message_queue_init(MessageQueue *queue, const char *name, UBaseType_t depth);

/**
 * @brief Deinitialize a MessageQueue, freeing its resources.
 * @param queue Pointer to the MessageQueue to deinitialize.
 */
void message_queue_deinit(MessageQueue *queue);

/**
 * @brief Send a message pointer to the specified MessageQueue.
 * @param queue Pointer to the MessageQueue to send data to.
 * @param sender Metadata describing which task produced the message.
 * @param data Opaque pointer passed through the queue unchanged.
 * @param timeout Maximum time to wait for space in the queue (in ticks).
 * @return true if the message was sent successfully, false otherwise.
 */
bool message_queue_send(MessageQueue *queue, Sender sender, void *data, TickType_t timeout);

/**
 * @brief Receive a message from the specified MessageQueue.
 * @param queue Pointer to the MessageQueue to receive the message from.
 * @param out_message Pointer to a Message structure to store the received message.
 * @param timeout Maximum time to wait for a message in the queue (in ticks).
 * @return true if a message was received successfully, false otherwise.
 */
bool message_queue_receive(MessageQueue *queue, Message *out_message, TickType_t timeout);

#ifdef __cplusplus
}
#endif

#endif // MESSAGE_QUEUE_H