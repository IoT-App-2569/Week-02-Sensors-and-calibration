#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_adc/adc_oneshot.h" // <-- เปลี่ยนมาใช้ไดรเวอร์ใหม่ตัวนี้

#define ADC_MIN 180
#define ADC_MAX 3900
#define DEG_MAX 180.0

float adc_to_angle(int adc)
{
    // ป้องกันกรณีค่า ADC ต่ำกว่า MIN จนมุมติดลบ
    if (adc < ADC_MIN) adc = ADC_MIN;
    // ป้องกันกรณีค่า ADC สูงกว่า MAX จนมุมเกิน 180
    if (adc > ADC_MAX) adc = ADC_MAX;
    
    return (adc - ADC_MIN) * DEG_MAX / (ADC_MAX - ADC_MIN);
}

void app_main(void)
{
    // 1. เริ่มต้นระบบตัวควบคุม ADC (Unit 1)
    adc_oneshot_unit_handle_t adc1_handle;
    adc_oneshot_unit_init_cfg_t init_config1 = {
        .unit_id = ADC_UNIT_1,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &adc1_handle));

    // 2. ตั้งค่าความละเอียด 12-bit และการลดทอนสัญญาณสัญญาณ (เทียบเท่า DB_11 เดิม)
    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_12,
        .atten = ADC_ATTEN_DB_12, // บน IDF v6 แนะนำให้เขียนเป็น DB_12 ครับ สเกลเดียวกัน
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, ADC_CHANNEL_6, &config));

    while (1) {
        int adc = 0;
        // 3. อ่านค่าดิบ (Raw value) จาก ADC Channel 6
        ESP_ERROR_CHECK(adc_oneshot_read(adc1_handle, ADC_CHANNEL_6, &adc));

        // คำนวณหามุมจากฟังก์ชันเดิมของคุณ
        float angle = adc_to_angle(adc);

        printf("ADC = %d  Angle = %.2f deg\n", adc, angle);
        vTaskDelay(pdMS_TO_TICKS(300));
    }
}