#include <stdio.h>

#include "esp_adc/adc_oneshot.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define ADC_MIN 267.0
#define ADC_MAX 3071.0
#define DEG_MAX 180.0

float adc_to_angle(int adc)
{
    return (adc - ADC_MIN) * DEG_MAX / (ADC_MAX - ADC_MIN);
}

void app_main(void)
{
    adc_oneshot_unit_handle_t adc_handle;
    adc_oneshot_unit_init_cfg_t init_cfg = {
        .unit_id = ADC_UNIT_1,
        .ulp_mode = ADC_ULP_MODE_DISABLE,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_cfg, &adc_handle));

    adc_oneshot_chan_cfg_t chan_cfg = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = ADC_ATTEN_DB_12,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc_handle, ADC_CHANNEL_6, &chan_cfg));

    while (1) {
        int adc = 0;
        ESP_ERROR_CHECK(adc_oneshot_read(adc_handle, ADC_CHANNEL_6, &adc));
        float angle = adc_to_angle(adc);

        printf("ADC = %d  Angle = %.2f deg\n", adc, angle);
        vTaskDelay(pdMS_TO_TICKS(300));
    }
}