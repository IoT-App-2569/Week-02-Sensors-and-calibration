#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_adc/adc_oneshot.h" 

void app_main(void)
{
    // 1. ตั้งค่าและสร้าง ADC Unit Handle (ใช้ _cfg_t)
    adc_oneshot_unit_handle_t adc1_handle;
    adc_oneshot_unit_init_cfg_t init_config1 = {
        .unit_id = ADC_UNIT_1,
        .ulp_mode = ADC_ULP_MODE_DISABLE,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &adc1_handle));

    // 2. ตั้งค่า Channel และ Attenuation (ใช้ _cfg_t และเปลี่ยนแมปค่าตาม v6)
    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_DEFAULT, // v6 แนะนำใช้ตัวนี้จะเลือกค่าสูงสุดให้อัตโนมัติ (12-bit)
        .atten = ADC_ATTEN_DB_12,         // สำหรับ ESP32 โหมดช่วงกว้างสุดใช้ 12dB แทน 11dB เดิม
    };
    
    // ตั้งค่าสำหรับ ADC1 Channel 6
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, ADC_CHANNEL_6, &config));

    while (1) {
        int adc_raw;
        // 3. อ่านค่า ADC
        ESP_ERROR_CHECK(adc_oneshot_read(adc1_handle, ADC_CHANNEL_6, &adc_raw));
        
        printf("ADC = %d\n", adc_raw);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}