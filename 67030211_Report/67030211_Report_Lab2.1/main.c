#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"

void app_main(void)
{
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ADC1_CHANNEL_6, ADC_ATTEN_DB_11);

    while (1) {
        int adc = adc1_get_raw(ADC1_CHANNEL_6);
        printf("ADC = %d\n", adc);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}
