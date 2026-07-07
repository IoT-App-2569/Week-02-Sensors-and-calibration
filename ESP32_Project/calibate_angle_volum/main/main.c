#include <stdio.h>

#include "esp_adc/adc_oneshot.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define ADC_MIN 0
#define ADC_MAX 2360
#define DEG_MAX 180.0


float adc_to_angle(int adc)
{
    // จำกัดค่า adc ไม่ให้หลุดขอบเขต
    if (adc < ADC_MIN) adc = ADC_MIN;
    if (adc > ADC_MAX) adc = ADC_MAX;

    return (adc - ADC_MIN) * DEG_MAX / (ADC_MAX - ADC_MIN);
}


int snap_to_step(float angle)
{
    // ปัดมุมให้เข้ากลุ่มที่ใกล้ที่สุดใน 0, 45, 90, 135, 180
    int steps[] = {0, 45, 90, 135, 180};
    int closest = steps[0];
    float min_diff = 1e9;

    for (int i = 0; i < 5; i++)
    {
        float diff = angle - steps[i];
        if (diff < 0) diff = -diff;

        if (diff < min_diff)
        {
            min_diff = diff;
            closest = steps[i];
        }
    }

    return closest;
}


void app_main(void)
{
    adc_oneshot_unit_handle_t adc_handle;

    adc_oneshot_unit_init_cfg_t init_config = {
        .unit_id = ADC_UNIT_1,
    };

    adc_oneshot_new_unit(&init_config, &adc_handle);

    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_12,
        .atten = ADC_ATTEN_DB_12,
    };

    // GPIO34 = ADC1 Channel 6
    adc_oneshot_config_channel(
        adc_handle,
        ADC_CHANNEL_6,
        &config
    );

    while (1)
    {
        int adc_value;

        adc_oneshot_read(
            adc_handle,
            ADC_CHANNEL_6,
            &adc_value
        );

        float angle = adc_to_angle(adc_value);
        int snapped = snap_to_step(angle);

        printf("ADC = %d  Angle = %.2f deg  Snapped = %d deg\n",
               adc_value,
               angle,
               snapped);

        vTaskDelay(pdMS_TO_TICKS(300));
    }
}