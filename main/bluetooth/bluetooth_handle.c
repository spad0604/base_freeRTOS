#include "bluetooth_handle.h"
#include "sdkconfig.h"

#if defined(CONFIG_IDF_TARGET_ESP32) && defined(CONFIG_BT_BLE_ENABLED)

#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_gap_ble_api.h"
#include "esp_gatt_common_api.h"
#include "nvs_flash.h"
#include <string.h>

#include "app_config.h"
#include "os/os_log.h"

static const char *TAG = "BLE";
static bool ble_started = false;

static esp_ble_adv_data_t adv_data = {
    .set_scan_rsp = false,
    .include_name = true,
    .include_txpower = true,
    .min_interval = 0x0006, /* placeholder, overwritten in start */
    .max_interval = 0x0010, /* placeholder, overwritten in start */
    .appearance = 0x00,
    .manufacturer_len = 0,
    .p_manufacturer_data = NULL,
    .service_data_len = 0,
    .p_service_data = NULL,
    .service_uuid_len = 0,
    .p_service_uuid = NULL,
    .flag = (ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT),
};

static esp_ble_adv_params_t adv_params = {
    .adv_int_min = 0x20,
    .adv_int_max = 0x40,
    .adv_type = ADV_TYPE_IND,
    .own_addr_type = BLE_ADDR_TYPE_PUBLIC,
    .channel_map = ADV_CHNL_ALL,
    .adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
};

static uint16_t ms_to_ble_units(uint32_t ms)
{
    /* 0.625 ms per BLE unit -> ms * 1000 / 625 */
    return (uint16_t)((ms * 1000U) / 625U);
}

static void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
{
    switch (event)
    {
    case ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT:
        esp_ble_gap_start_advertising(&adv_params);
        break;
    case ESP_GAP_BLE_ADV_START_COMPLETE_EVT:
        if (param->adv_start_cmpl.status == ESP_BT_STATUS_SUCCESS)
        {
            os_log_print(OS_LOG_LEVEL_INFO, TAG, "Advertising started");
        }
        else
        {
            os_log_print(OS_LOG_LEVEL_ERROR, TAG, "Failed to start advertising: %d", param->adv_start_cmpl.status);
        }
        break;
    case ESP_GAP_BLE_ADV_STOP_COMPLETE_EVT:
        os_log_print(OS_LOG_LEVEL_INFO, TAG, "Advertising stopped (status=%d)", param->adv_stop_cmpl.status);
        break;
    default:
        break;
    }
}

esp_err_t bluetooth_handle_start(void)
{
    if (ble_started)
    {
        return ESP_OK;
    }

    /* Ensure storage ready for BT stack */
    ESP_ERROR_CHECK(nvs_flash_init());

    /* Release classic BT memory to save RAM */
    esp_err_t err = esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT);
    if (err != ESP_OK && err != ESP_ERR_INVALID_STATE)
    {
        return err;
    }

    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_bt_controller_init(&bt_cfg));
    ESP_ERROR_CHECK(esp_bt_controller_enable(ESP_BT_MODE_BLE));
    ESP_ERROR_CHECK(esp_bluedroid_init());
    ESP_ERROR_CHECK(esp_bluedroid_enable());

    ESP_ERROR_CHECK(esp_ble_gap_register_callback(gap_event_handler));
    ESP_ERROR_CHECK(esp_ble_gap_set_device_name(APP_CFG_BLE_DEVICE_NAME));

    uint16_t adv_min = ms_to_ble_units(APP_CFG_BLE_ADV_INT_MIN_MS);
    uint16_t adv_max = ms_to_ble_units(APP_CFG_BLE_ADV_INT_MAX_MS);

    /* Ensure intervals within spec (0x0020 - 0x4000) and min <= max */
    if (adv_min < 0x20)
    {
        adv_min = 0x20;
    }
    if (adv_max < adv_min)
    {
        adv_max = adv_min;
    }
    adv_data.min_interval = adv_min;
    adv_data.max_interval = adv_max;
    adv_params.adv_int_min = adv_min;
    adv_params.adv_int_max = adv_max;

    ESP_ERROR_CHECK(esp_ble_gap_config_adv_data(&adv_data));

    ble_started = true;
    os_log_print(OS_LOG_LEVEL_INFO, TAG, "BLE initialized (name: %s)", APP_CFG_BLE_DEVICE_NAME);
    return ESP_OK;
}

esp_err_t bluetooth_handle_stop(void)
{
    if (!ble_started)
    {
        return ESP_OK;
    }

    esp_ble_gap_stop_advertising();
    esp_bluedroid_disable();
    esp_bluedroid_deinit();
    esp_bt_controller_disable(ESP_BT_MODE_BLE);
    esp_bt_controller_deinit();
    ble_started = false;
    os_log_print(OS_LOG_LEVEL_INFO, TAG, "BLE stopped");
    return ESP_OK;
}

#endif /* CONFIG_IDF_TARGET_ESP32 && CONFIG_BT_BLE_ENABLED */

