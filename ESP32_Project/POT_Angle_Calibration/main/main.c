#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"
#include "esp_log.h"

#define ADC_CHANNEL     ADC1_CHANNEL_6   // GPIO34
#define ADC_ATTEN       ADC_ATTEN_DB_11

// Calibration constants from Step 4 - update after measuring your own POT
#define ADC_MIN 180
#define ADC_MAX 3900
#define DEG_MAX 180.0f

static float adc_to_angle(int adc)
{
    return (adc - ADC_MIN) * DEG_MAX / (ADC_MAX - ADC_MIN);
}

void app_main(void)
{
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ADC_CHANNEL, ADC_ATTEN);

    while (1) {
        int adc = adc1_get_raw(ADC_CHANNEL);
        float angle = adc_to_angle(adc);

        printf("ADC = %4d  Angle = %6.2f deg\n", adc, angle);
        vTaskDelay(pdMS_TO_TICKS(300));
    }
}
