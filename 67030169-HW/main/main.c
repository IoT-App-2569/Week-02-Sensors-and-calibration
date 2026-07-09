#include <stdio.h>
#include "freertos/FreeRTOS.h"  // <--- เพิ่ม: ห้ามลืมเด็ดขาด ไม่งั้น Error
#include "freertos/task.h"      // <--- เพิ่ม: ห้ามลืมเด็ดขาด ไม่งั้น Error
#include "driver/adc.h"
#include "esp_log.h"

#define ADC_MIN 180
#define ADC_MAX 3900
#define DEG_MAX 180.0f

float adc_to_angle(int adc)
{
    // ป้องกันกรณีค่า ADC ต่ำกว่า MIN หรือสูงกว่า MAX หลุดขอบเข้ามา
    if (adc < ADC_MIN) adc = ADC_MIN;
    if (adc > ADC_MAX) adc = ADC_MAX;

    // บังคับให้เป็น float (ใส่ .0f) เพื่อให้คำนวณทศนิยมได้แม่นยำ ไม่โดนปัดเศษทิ้ง
    return ((float)(adc - ADC_MIN) * DEG_MAX) / (float)(ADC_MAX - ADC_MIN);
}

void app_main(void)
{
    // กำหนดความละเอียด ADC = 12 bit (0–4095)
    adc1_config_width(ADC_WIDTH_BIT_12);
    
    // เปลี่ยนเป็น DB_12 เพื่อรองรับ ESP-IDF v5.x
    adc1_config_channel_atten(ADC1_CHANNEL_6, ADC_ATTEN_DB_12);

    while (1) {
        int adc = adc1_get_raw(ADC1_CHANNEL_6);
        float angle = adc_to_angle(adc);

        printf("ADC = %d  Angle = %.2f deg\n", adc, angle);
        vTaskDelay(pdMS_TO_TICKS(300));
    }
}