#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_adc/adc_oneshot.h"

#define ADC_MIN 180
#define ADC_MAX 3900
#define DEG_MAX 180.0

float adc_to_angle(int adc)
{
    float angle = (adc - ADC_MIN) * DEG_MAX / (ADC_MAX - ADC_MIN);

    if (angle < 0)
        angle = 0;
    if (angle > 180)
        angle = 180;

    return angle;
}

void app_main(void)
{
    adc_oneshot_unit_handle_t adc_handle;

    adc_oneshot_unit_init_cfg_t init_config = {
        .unit_id = ADC_UNIT_1,
    };

    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config, &adc_handle));

    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = ADC_ATTEN_DB_12,
    };

    ESP_ERROR_CHECK(adc_oneshot_config_channel(
        adc_handle,
        ADC_CHANNEL_6,
        &config));

    int adc;

    while (1)
    {
        ESP_ERROR_CHECK(adc_oneshot_read(
            adc_handle,
            ADC_CHANNEL_6,
            &adc));

        float angle = adc_to_angle(adc);

        printf("ADC = %4d   Angle = %.2f deg\n", adc, angle);

        vTaskDelay(pdMS_TO_TICKS(300));
    }
}