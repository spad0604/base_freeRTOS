#include "http_request.h"
#include "app_config.h"
#include "os/os_log.h"

#include "stdlib.h"
#include "string.h"

const char *TAG = "HTTP";

#ifdef CONFIG_IDF_TARGET_ESP32
#include "esp_http_client.h"
#include "esp_err.h"

typedef struct
{
    net_data_cb_t cb;
    void *ctx;
    const char *url;
} _http_user_t;

static esp_err_t _esp_http_event_handler(esp_http_client_event_t *evt)
{
    _http_user_t *u = (_http_user_t *)esp_http_client_get_user_data(evt->client);
    switch (evt->event_id)
    {
    case HTTP_EVENT_ON_DATA:
        if (u && u->cb && evt->data && evt->data_len > 0)
        {
            u->cb(u->url ? u->url : "", (const uint8_t *)evt->data, evt->data_len, u->ctx);
        }
        break;

    default:
        break;
    }

    return ESP_OK;
}

net_status_t http_request_async(
    const char *method,
    const char *url,
    const uint8_t *body,
    uint32_t timeout_ms,
    net_data_cb_t on_response,
    void *ctx)
{
    if (!method || !url)
        return NET_ERR;
    esp_http_client_config_t config = {
        .url = url,
        .event_handler = _esp_http_event_handler,
        .timeout_ms = (int)timeout_ms,
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);
    if (!client)
        return NET_ERR;

    _http_user_t u = {.cb = on_response, .ctx = ctx, .url = url};
    esp_http_client_set_user_data(client, &u);

    if (strcasecmp(method, "POST") == 0)
    {
        esp_http_client_set_method(client, HTTP_METHOD_POST);
        if (body)
        {
            esp_http_client_set_post_field(client, (const char *)body, (int)strlen((const char *)body));
        }
    }
    else
    {
        esp_http_client_set_method(client, HTTP_METHOD_GET);
    }

    esp_err_t err = esp_http_client_perform(client);
    if (err != ESP_OK)
    {
        os_log_print(OS_LOG_LEVEL_ERROR, TAG, "HTTP request failed: %s", esp_err_to_name(err));
        esp_http_client_cleanup(client);
        return NET_ERR;
    }

    esp_http_client_cleanup(client);
    return NET_OK;
}

net_status_t http_request_blocking(const char *method, const char *url, const uint8_t *body, uint32_t timeout_ms, uint8_t **out_response, size_t *out_response_len)
{
    if (!method || !url || !out_response || !out_response_len)
        return NET_ERR;
    *out_response = NULL;
    *out_response_len = 0;

    esp_http_client_config_t config = {
        .url = url,
        .timeout_ms = (int)timeout_ms,
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);
    if (!client)
        return NET_ERR;

    if (strcasecmp(method, "POST") == 0)
    {
        esp_http_client_set_method(client, HTTP_METHOD_POST);
        if (body)
            esp_http_client_set_post_field(client, (const char *)body, (int)strlen((const char *)body));
    }
    else
    {
        esp_http_client_set_method(client, HTTP_METHOD_GET);
    }

    esp_err_t err = esp_http_client_perform(client);
    if (err != ESP_OK)
    {
        os_log_print(OS_LOG_LEVEL_ERROR, TAG, "perform fail: %d", err);
        esp_http_client_cleanup(client);
        return NET_ERR;
    }

    int len = esp_http_client_get_content_length(client);
    if (len > 0)
    {
        uint8_t *buf = malloc((size_t)len);
        if (!buf)
        {
            esp_http_client_cleanup(client);
            return NET_ERR;
        }
        int read = esp_http_client_read_response(client, (char *)buf, len);
        if (read > 0)
        {
            *out_response = buf;
            *out_response_len = (size_t)read;
        }
        else
        {
            free(buf);
        }
    }

    esp_http_client_cleanup(client);
    return NET_OK;
}
else // Non-ESP32 platforms

net_status_t http_request_async(const char* method, const char* url, const uint8_t *body, uint32_t timeout_ms, net_data_cb_t on_response, void *ctx)
{
    (void)method; (void)url; (void)body; (void)timeout_ms; (void)on_response; (void)ctx;
    os_log_print(OS_LOG_LEVEL_WARNING, "HTTP", "http_request_async: not implemented on this platform");
    return NET_NOT_IMPLEMENTED;
}

net_status_t http_request_blocking(const char* method, const char* url, const uint8_t *body, uint32_t timeout_ms, uint8_t **out_response, size_t *out_response_len)
{
    (void)method; (void)url; (void)body; (void)timeout_ms; (void)out_response; (void)out_response_len;
    os_log_print(OS_LOG_LEVEL_WARNING, "HTTP", "http_request_blocking: not implemented on this platform");
    return NET_NOT_IMPLEMENTED;
}

#endif

net_status_t http_get_async(const char* url, uint32_t timeout_ms, net_data_cb_t on_response, void *ctx)
{
    return http_request_async("GET", url, NULL, timeout_ms, on_response, ctx);
}

net_status_t http_post_async(const char* url, const uint8_t *body, size_t body_len, uint32_t timeout_ms, net_data_cb_t on_response, void *ctx)
{
    (void)body_len; // esp_http_client_set_post_field infers length from string; caller should ensure body is NUL-terminated or adapt as needed
    return http_request_async("POST", url, body, timeout_ms, on_response, ctx);
}

net_status_t http_get_blocking(const char* url, uint32_t timeout_ms, uint8_t **out_response, size_t *out_response_len)
{
    return http_request_blocking("GET", url, NULL, timeout_ms, out_response, out_response_len);
}

net_status_t http_post_blocking(const char* url, const uint8_t *body, size_t body_len, uint32_t timeout_ms, uint8_t **out_response, size_t *out_response_len)
{
    (void)body_len;
    return http_request_blocking("POST", url, body, timeout_ms, out_response, out_response_len);
}