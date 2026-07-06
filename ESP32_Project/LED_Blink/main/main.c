// ============================================================
// ขั้นตอนที่ 3 — อ่านค่า ADC ดิบ (commented out แล้ว)
// ============================================================
// #include <stdio.h>
// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"
// #include "esp_adc/adc_oneshot.h"
//
// void app_main(void)
// {
//     adc_oneshot_unit_handle_t adc1_handle;
//     adc_oneshot_unit_init_cfg_t init_config = {
//         .unit_id = ADC_UNIT_1,
//     };
//     adc_oneshot_new_unit(&init_config, &adc1_handle);
//
//     adc_oneshot_chan_cfg_t chan_config = {
//         .bitwidth = ADC_BITWIDTH_12,
//         .atten    = ADC_ATTEN_DB_12,
//     };
//     adc_oneshot_config_channel(adc1_handle, ADC_CHANNEL_6, &chan_config);
//
//     while (1) {
//         int adc_raw;
//         adc_oneshot_read(adc1_handle, ADC_CHANNEL_6, &adc_raw);
//         printf("ADC = %d\n", adc_raw);
//         vTaskDelay(pdMS_TO_TICKS(500));
//     }
// }

// ============================================================
// ขั้นตอนที่ 6 — แสดงผลมุมแบบเรียลไทม์ (Active)
// ⚠️ แก้ ADC_MIN และ ADC_MAX ให้ตรงกับค่าที่วัดได้จริงในขั้นตอนที่ 4
// ============================================================
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_adc/adc_oneshot.h"

#define ADC_MIN 200      // ค่า ADC ที่มุม 0°  (แก้ตามที่วัดได้จริง)
#define ADC_MAX 4095     // ค่า ADC ที่มุม 180° (แก้ตามที่วัดได้จริง)
#define DEG_MAX 180.0    // มุมสูงสุด (องศา)

float adc_to_angle(int adc)
{
    return (float)(adc - ADC_MIN) * DEG_MAX / (ADC_MAX - ADC_MIN);
}

void app_main(void)
{
    // ตั้งค่า ADC Unit 1
    adc_oneshot_unit_handle_t adc1_handle;
    adc_oneshot_unit_init_cfg_t init_config = {
        .unit_id = ADC_UNIT_1,
    };
    adc_oneshot_new_unit(&init_config, &adc1_handle);

    // ตั้งค่า Channel 6 (GPIO34)
    adc_oneshot_chan_cfg_t chan_config = {
        .bitwidth = ADC_BITWIDTH_12,
        .atten    = ADC_ATTEN_DB_12,
    };
    adc_oneshot_config_channel(adc1_handle, ADC_CHANNEL_6, &chan_config);

    while (1) {
        int adc_raw;
        adc_oneshot_read(adc1_handle, ADC_CHANNEL_6, &adc_raw);
        float angle = adc_to_angle(adc_raw);

        printf("ADC = %d  Angle = %.2f deg\n", adc_raw, angle);
        vTaskDelay(pdMS_TO_TICKS(300));
    }
}