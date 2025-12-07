#ifndef WIFI_HANDLE_H
#define WIFI_HANDLE_H

#include "os/os_log.h"
#include "sdkconfig.h"
#include <stdint.h>

#ifdef CONFIG_IDF_TARGET_ESP32
#include "esp_err.h"
#include "esp_event.h"

/**
 * @brief Event handler for WiFi events.
 * @param arg User-defined argument.
 * @param event_base Event base.
 * @param event_id Event ID.
 * @param event_data Event data.
 */
void wifi_handle_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);

/**
 * @brief Start WiFi station mode.
 * @return ESP_OK on success, ESP_ERR_INVALID_STATE if WiFi is not initialized.
 */
esp_err_t wifi_handle_start_sta(void);

/**
 * @brief Stop WiFi station mode.
 * @return ESP_OK on success, ESP_ERR_INVALID_STATE if WiFi is not initialized.
 */
esp_err_t wifi_handle_stop(void);

#endif

#endif // WIFI_HANDLE_H