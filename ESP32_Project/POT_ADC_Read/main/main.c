#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_log.h"

void app_main(void)
{
    // 1. สร้าง handle และกำหนดค่าเริ่มต้นให้กับ ADC1 One-Shot unit
    adc_oneshot_unit_handle_t adc1_handle;
    adc_oneshot_unit_init_cfg_t init_config1 = {
        .unit_id = ADC_UNIT_1,
    };
    adc_oneshot_new_unit(&init_config1, &adc1_handle);

    // 2. กำหนด channel 6 (GPIO34) และ attenuation (ADC_ATTEN_DB_12 สำหรับช่วง ~0-3.3V ใน IDF v6)
    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = ADC_ATTEN_DB_12,
    };
    adc_oneshot_config_channel(adc1_handle, ADC_CHANNEL_6, &config);

    while (1) {
        int adc = 0;
        adc_oneshot_read(adc1_handle, ADC_CHANNEL_6, &adc);
        printf("ADC = %d\n", adc);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}
