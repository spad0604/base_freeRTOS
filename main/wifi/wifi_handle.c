#include "wifi_handle.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "lwip/inet.h"
#include "sdkconfig.h"
#include <string.h>
#include "os/os_log.h"
#include "app_config.h"

/* Provide sensible defaults for static analysis when sdkconfig is not visible. */
#ifndef CONFIG_ESP_WIFI_STATIC_RX_BUFFER_NUM
#define CONFIG_ESP_WIFI_STATIC_RX_BUFFER_NUM 10
#endif
#ifndef CONFIG_ESP_WIFI_DYNAMIC_RX_BUFFER_NUM
#define CONFIG_ESP_WIFI_DYNAMIC_RX_BUFFER_NUM 32
#endif
#ifndef CONFIG_ESP_WIFI_STATIC_TX_BUFFER_NUM
#define CONFIG_ESP_WIFI_STATIC_TX_BUFFER_NUM 0
#endif
#ifndef CONFIG_ESP_WIFI_DYNAMIC_TX_BUFFER_NUM
#define CONFIG_ESP_WIFI_DYNAMIC_TX_BUFFER_NUM 32
#endif
#ifndef CONFIG_ESP_WIFI_CSI_ENABLED
#define CONFIG_ESP_WIFI_CSI_ENABLED 0
#endif
#ifndef CONFIG_ESP_WIFI_AMPDU_RX_ENABLED
#define CONFIG_ESP_WIFI_AMPDU_RX_ENABLED 1
#endif
#ifndef CONFIG_ESP_WIFI_AMPDU_TX_ENABLED
#define CONFIG_ESP_WIFI_AMPDU_TX_ENABLED 1
#endif
#ifndef CONFIG_ESP_WIFI_AMSDU_TX_ENABLED
#define CONFIG_ESP_WIFI_AMSDU_TX_ENABLED 1
#endif
#ifndef CONFIG_ESP_WIFI_RX_BA_WIN
#define CONFIG_ESP_WIFI_RX_BA_WIN 6
#endif
#ifndef CONFIG_ESP_WIFI_NVS_ENABLED
#define CONFIG_ESP_WIFI_NVS_ENABLED 1
#endif
#ifndef CONFIG_ESP_WIFI_SOFTAP_SUPPORT
#define CONFIG_ESP_WIFI_SOFTAP_SUPPORT 1
#endif
#ifndef CONFIG_ESP_WIFI_TX_BUFFER_TYPE
#define CONFIG_ESP_WIFI_TX_BUFFER_TYPE 1
#endif

const char *TAG = "WIFI";

static bool connected = false;
static char ip_str[16];

void wifi_handle_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
    {
        esp_wifi_connect();
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        connected = false;
        esp_wifi_connect();
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        esp_ip4addr_ntoa(&event->ip_info.ip, ip_str, sizeof(ip_str));
        connected = true;
        os_log_print(OS_LOG_LEVEL_INFO, TAG, "WiFi connected to %s", ip_str);
    }
}

esp_err_t wifi_handle_start_sta(void)
{
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_ERROR_CHECK(esp_netif_init());
    ret = esp_event_loop_create_default();
    if (ret != ESP_OK && ret != ESP_ERR_INVALID_STATE)
    {
        return ret;
    }
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    // Register event handlers
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_handle_event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, wifi_handle_event_handler, NULL));

    wifi_config_t wifi_config = {
        .sta = {
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,
            .pmf_cfg = {
                .capable = true,
                .required = false
            }
        }
    };

    /* Cast to char* to satisfy signature (fields are uint8_t in IDF). Use strlcpy to ensure termination. */
    strlcpy((char *)wifi_config.sta.ssid, APP_CFG_WIFI_SSID, sizeof(wifi_config.sta.ssid));
    strlcpy((char *)wifi_config.sta.password, APP_CFG_WIFI_PASSWORD, sizeof(wifi_config.sta.password));

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    os_log_print(OS_LOG_LEVEL_INFO, TAG, "WiFi started");

    return ESP_OK;
}

esp_err_t wifi_handle_stop(void)
{
    esp_wifi_stop();
    esp_wifi_deinit();
    connected = false;
    ip_str[0] = '\0';

    return ESP_OK;
}