#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_err.h"
 
#define ADC_CHANNEL ADC_CHANNEL_6      // GPIO34 on ESP32 (ADC1)
#define ADC_ATTENUATION ADC_ATTEN_DB_12
#define ADC_WIDTH ADC_BITWIDTH_12
 
// แก้ค่าเหล่านี้ให้เป็นค่า ADC จริงจากการทดลองของคุณ
// ตัวอย่าง: 0°, 45°, 90°, 135°, 180°
static const int adc_cal_points[] = {180, 1000, 2100, 3000, 3900};
static const float angle_cal_points[] = {0.0f, 45.0f, 90.0f, 135.0f, 180.0f};
#define CALIB_POINTS (sizeof(adc_cal_points) / sizeof(adc_cal_points[0]))
 
static float adc_to_angle(int raw)
{
    if (raw <= adc_cal_points[0]) {
        return angle_cal_points[0];
    }
 
    if (raw >= adc_cal_points[CALIB_POINTS - 1]) {
        return angle_cal_points[CALIB_POINTS - 1];
    }
 
    for (int i = 0; i < CALIB_POINTS - 1; ++i) {
        if (raw >= adc_cal_points[i] && raw <= adc_cal_points[i + 1]) {
            int x0 = adc_cal_points[i];
            int x1 = adc_cal_points[i + 1];
            float y0 = angle_cal_points[i];
            float y1 = angle_cal_points[i + 1];
 
            if (x1 == x0) {
                return y0;
            }
 
            return y0 + (float)(raw - x0) * (y1 - y0) / (float)(x1 - x0);
        }
    }
 
    return angle_cal_points[CALIB_POINTS - 1];
}
 
void app_main(void)
{
    adc_oneshot_unit_handle_t adc1_handle;
    adc_oneshot_unit_init_cfg_t init_config1 = {
        .unit_id = ADC_UNIT_1,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &adc1_handle));
 
    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_WIDTH,
        .atten = ADC_ATTENUATION,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, ADC_CHANNEL, &config));
 
    while (1) {
        int adc_value = 0;
        ESP_ERROR_CHECK(adc_oneshot_read(adc1_handle, ADC_CHANNEL, &adc_value));
 
        float angle = adc_to_angle(adc_value);
        printf("ADC = %d  Angle = %.1f deg\n", adc_value, angle);
 
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}