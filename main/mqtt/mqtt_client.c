#include "mqtt_wrapper.h"
#include "app_config.h"
#include "os/os_log.h"

const char *TAG = "MQTT";

#ifdef CONFIG_IDF_TARGET_ESP32
#include <mqtt_client.h>
#include <esp_err.h>
#include <esp_event.h>

static esp_mqtt_client_handle_t s_mqtt = NULL;

static void _mqtt_event_handler_cb(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    (void)handler_args;
    (void)base;
    esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t)event_data;

    switch ((esp_mqtt_event_id_t)event_id)
    {
    case MQTT_EVENT_DATA:
        if (event->data && event->topic)
        {
            os_log_print(OS_LOG_LEVEL_INFO, TAG, "msg topic=%.*s len=%d", event->topic_len, event->topic, event->data_len);
        }
        break;
    case MQTT_EVENT_CONNECTED:
        os_log_print(OS_LOG_LEVEL_INFO, TAG, "MQTT connected");
        break;
    case MQTT_EVENT_DISCONNECTED:
        os_log_print(OS_LOG_LEVEL_INFO, TAG, "MQTT disconnected");
        break;
    default:
        break;
    }
}

net_status_t mqtt_client_init_and_start(const mqtt_config_t *cfg)
{
    if (!cfg || !cfg->broker_url)
        return NET_ERR;

    esp_mqtt_client_config_t mcfg = {
        .broker.address.uri = cfg->broker_url,
        .credentials.client_id = cfg->client_id,
        .credentials.username = cfg->username,
        .credentials.authentication.password = cfg->password,
    };

    s_mqtt = esp_mqtt_client_init(&mcfg);
    if (!s_mqtt)
        return NET_ERR;

    esp_mqtt_client_register_event(s_mqtt, ESP_EVENT_ANY_ID, _mqtt_event_handler_cb, NULL);
    esp_mqtt_client_start(s_mqtt);
    return NET_OK;
}

void mqtt_client_stop(void)
{
    if (s_mqtt)
    {
        esp_mqtt_client_stop(s_mqtt);
        esp_mqtt_client_destroy(s_mqtt);
        s_mqtt = NULL;
    }
}

net_status_t mqtt_publish(const char *topic, const uint8_t *payload, size_t len, int qos)
{
    if (!s_mqtt || !topic)
        return NET_ERR;
    int msg_id = esp_mqtt_client_publish(s_mqtt, topic, (const char *)payload, len, qos, 0);
    return (msg_id >= 0) ? NET_OK : NET_ERR;
}

net_status_t mqtt_subscribe(const char *topic, int qos, net_data_cb_t on_message, void *ctx)
{
    (void)on_message;
    (void)ctx; // advanced routing not implemented in this simple wrapper
    if (!s_mqtt || !topic)
        return NET_ERR;
    int rc = esp_mqtt_client_subscribe(s_mqtt, topic, qos);
    return (rc >= 0) ? NET_OK : NET_ERR;
}

#else

net_status_t mqtt_client_init_and_start(const mqtt_config_t *cfg)
{
    (void)cfg;
    os_log_print(OS_LOG_LEVEL_WARN, TAG, "mqtt not implemented on this platform");
    return NET_NOT_IMPLEMENTED;
}
void mqtt_client_stop(void) {}
net_status_t mqtt_publish(const char *topic, const uint8_t *payload, size_t len, int qos)
{
    (void)topic;
    (void)payload;
    (void)len;
    (void)qos;
    return NET_NOT_IMPLEMENTED;
}
net_status_t mqtt_subscribe(const char *topic, int qos, net_data_cb_t on_message, void *ctx)
{
    (void)topic;
    (void)qos;
    (void)on_message;
    (void)ctx;
    return NET_NOT_IMPLEMENTED;
}

#endif