#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_adc/adc_oneshot.h"

static const char *TAG = "ADC_TEST";

void app_main(void)
{
    adc_oneshot_unit_handle_t adc1_handle;

    adc_oneshot_unit_init_cfg_t init_config = {
        .unit_id = ADC_UNIT_1,
    };

    adc_oneshot_new_unit(&init_config, &adc1_handle);

    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = ADC_ATTEN_DB_12,
    };

    // ตัวอย่างใช้ GPIO34 (ADC1 Channel 6)
    adc_oneshot_config_channel(
        adc1_handle,
        ADC_CHANNEL_6,
        &config
    );

    while (1)
    {
        int adc_value = 0;

        adc_oneshot_read(
            adc1_handle,
            ADC_CHANNEL_6,
            &adc_value
        );

        ESP_LOGI(TAG, "ADC Raw Value: %d", adc_value);

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}