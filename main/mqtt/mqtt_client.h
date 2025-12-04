#ifndef MQTT_CLIENT_H
#define MQTT_CLIENT_H

#include "net/net_client.h"
#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct
    {
        const char *broker_url;
        const char *client_id;
        const char *username;
        const char *password;
        void *user_ctx;
    } mqtt_config_t;

    /**
     * @brief Initialize and start the MQTT client with the given configuration.
     * @param config Pointer to the MQTT configuration structure.
     */
    net_status_t mqtt_client_init_and_start(const mqtt_config_t *config);

    /**
     * @brief Stop the MQTT client and release its resources.
     */
    void mqtt_client_stop(void);

    /**
     * @brief Publish a message to the specified MQTT topic.
     * @param topic The MQTT topic to publish to.
     * @param payload Pointer to the message payload.
     * @param payload_len Length of the message payload in bytes.
     * @param qos Quality of Service level (0, 1, or 2).
     */
    net_status_t mqtt_publish(
        const char *topic,
        const uint8_t *payload,
        size_t payload_len,
        int qos);

    /**
     * @brief Subscribe to the specified MQTT topic.
     * @param topic The MQTT topic to subscribe to.
     * @param qos Quality of Service level (0, 1, or 2).
     * @param on_message Callback function to handle incoming messages.
     * @param ctx User-defined context pointer passed to the callback.
     */
    net_status_t mqtt_subscribe(
        const char *topic,
        int qos,
        net_data_cb_t on_message,
        void *ctx);

#ifdef __cplusplus
}

#endif

#endif // MQTT_CLIENT_H