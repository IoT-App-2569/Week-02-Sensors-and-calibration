#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_adc/adc_oneshot.h" // <-- ใช้ไดรเวอร์ตัวใหม่นี้แทน driver/adc.h

void app_main(void)
{
    // 1. สร้าง Handle สำหรับตัวควบคุม ADC (Unit 1)
    adc_oneshot_unit_handle_t adc1_handle;
    adc_oneshot_unit_init_cfg_t init_config1 = {
        .unit_id = ADC_UNIT_1,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &adc1_handle));

    // 2. ตั้งค่าความละเอียด (Bitwidth) และความแรงสัญญาณ (Attenuation) ให้กับ Channel 6
    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_12,       // ความละเอียด 12-bit (0-4095)
        .atten = ADC_ATTEN_DB_12,          // สัญญาณลดทอนสูงสุดเพื่อรับแรงดันสูงขึ้น (เทียบเท่า DB_11 เดิม)
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, ADC_CHANNEL_6, &config));

    while (1) {
        int adc_raw;
        // 3. สั่งอ่านค่าจาก ADC (เติม &adc_raw เข้าไปเพื่อรับค่า)
        ESP_ERROR_CHECK(adc_oneshot_read(adc1_handle, ADC_CHANNEL_6, &adc_raw));
        
        // พิมพ์ค่าที่อ่านได้ออกทาง Serial Monitor
        printf("ADC Raw Value: %d\n", adc_raw);
        
        // ดีเลย์ 1 วินาที (1000 มิลลิวินาที) จะได้ไม่ดึง CPU มากเกินไป
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}