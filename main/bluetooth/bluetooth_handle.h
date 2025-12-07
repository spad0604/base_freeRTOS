#ifndef BLUETOOTH_HANDLE_H
#define BLUETOOTH_HANDLE_H

#include "sdkconfig.h"
#include "esp_err.h"

#if defined(CONFIG_IDF_TARGET_ESP32) && defined(CONFIG_BT_BLE_ENABLED)

/**
 * @brief Start BLE advertising with basic GAP config.
 * @return ESP_OK on success.
 */
esp_err_t bluetooth_handle_start(void);

/**
 * @brief Stop BLE advertising and tear down controller.
 * @return ESP_OK on success.
 */
esp_err_t bluetooth_handle_stop(void);

#else /* BLE not enabled/unsupported */

static inline esp_err_t bluetooth_handle_start(void)
{
    return ESP_ERR_NOT_SUPPORTED;
}

static inline esp_err_t bluetooth_handle_stop(void)
{
    return ESP_ERR_NOT_SUPPORTED;
}

#endif

#endif // BLUETOOTH_HANDLE_H

