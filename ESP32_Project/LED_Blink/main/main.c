#include "driver/adc.h"
#include "esp_log.h"

#define ADC_MIN 200
#define ADC_MAX 3110
#define DEG_MAX 180.0

float adc_to_angle(int adc)
{
    return (adc - ADC_MIN) * DEG_MAX / (ADC_MAX - ADC_MIN);
}

void app_main(void)
{
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ADC1_CHANNEL_6, ADC_ATTEN_DB_11);

    while (1) {
        int adc = adc1_get_raw(ADC1_CHANNEL_6);
        float angle = adc_to_angle(adc);

        printf("ADC = %d  Angle = %.2f deg\n", adc, angle);
        vTaskDelay(pdMS_TO_TICKS(300));
    }
}