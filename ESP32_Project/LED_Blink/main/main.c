#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_log.h"

static const char *TAG = "adc_test";
#define ADC_CHANNEL ADC_CHANNEL_6  // GPIO34

void app_main(void)
{
    adc_oneshot_unit_handle_t adc1_handle;
    adc_oneshot_unit_init_cfg_t init_config = {
        .unit_id = ADC_UNIT_1,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config, &adc1_handle));

    adc_oneshot_chan_cfg_t chan_config = {
        .bitwidth = ADC_BITWIDTH_12,
        .atten = ADC_ATTEN_DB_12,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, ADC_CHANNEL, &chan_config));

    while (1) {
        int adc = -1;
        esp_err_t err = adc_oneshot_read(adc1_handle, ADC_CHANNEL, &adc);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "adc_oneshot_read failed: %s", esp_err_to_name(err));
        }
        printf("ADC = %d\n", adc);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}
