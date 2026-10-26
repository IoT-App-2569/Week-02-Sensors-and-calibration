#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"

#define ADC_MIN     0
#define ADC_MAX     2730
#define DEG_MAX     180

int adc_to_angle(int adc)
{
    if (adc <= ADC_MIN) return 0;
    if (adc >= ADC_MAX) return DEG_MAX;
    float angle = ((float)(adc - ADC_MIN) * DEG_MAX) / (float)(ADC_MAX - ADC_MIN);
    return (int)(angle + 0.5f);
}

void app_main(void)
{
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ADC1_CHANNEL_6, ADC_ATTEN_DB_11);

    while (1) {
        int adc = adc1_get_raw(ADC1_CHANNEL_6);
        int angle = adc_to_angle(adc);
        printf("ADC = %d  Angle = %d deg\n", adc, angle);
        vTaskDelay(pdMS_TO_TICKS(300));
    }
}
