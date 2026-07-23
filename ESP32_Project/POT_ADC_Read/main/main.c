#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_adc/adc_oneshot.h"

#define NUM_POINTS 5
#define SAMPLES 16

// ตารางคาลิเบรตจริงที่วัดได้จากบอร์ดคุณ
static const int   adc_table[NUM_POINTS]   = {187, 865, 1565, 2359, 3105};
static const float angle_table[NUM_POINTS] = {0,   45,  90,   135,  180};

// แปลง ADC -> มุม โดยใช้ piecewise linear interpolation จากตารางจริง
float adc_to_angle(int adc)
{
    // ถ้าค่าต่ำ/สูงกว่าขอบเขตที่วัดไว้ ให้ clamp ไว้ที่ขอบ
    if (adc <= adc_table[0]) return angle_table[0];
    if (adc >= adc_table[NUM_POINTS - 1]) return angle_table[NUM_POINTS - 1];

    // หาว่า adc อยู่ระหว่างช่วงไหนในตาราง แล้วเทียบสัดส่วนในช่วงนั้น
    for (int i = 0; i < NUM_POINTS - 1; i++) {
        if (adc >= adc_table[i] && adc <= adc_table[i + 1]) {
            float ratio = (float)(adc - adc_table[i]) / (adc_table[i + 1] - adc_table[i]);
            return angle_table[i] + ratio * (angle_table[i + 1] - angle_table[i]);
        }
    }
    return 0; // ไม่ควรมาถึงตรงนี้
}

int read_adc_averaged(adc_oneshot_unit_handle_t handle)
{
    long sum = 0;
    for (int i = 0; i < SAMPLES; i++) {
        int val = 0;
        adc_oneshot_read(handle, ADC_CHANNEL_6, &val);
        sum += val;
        vTaskDelay(pdMS_TO_TICKS(2));
    }
    return (int)(sum / SAMPLES);
}

void app_main(void)
{
    adc_oneshot_unit_init_cfg_t init_config = {
        .unit_id = ADC_UNIT_1,
    };
    adc_oneshot_unit_handle_t adc1_handle;
    adc_oneshot_new_unit(&init_config, &adc1_handle);

    adc_oneshot_chan_cfg_t config = {
        .atten = ADC_ATTEN_DB_12,
        .bitwidth = ADC_BITWIDTH_12,
    };
    adc_oneshot_config_channel(adc1_handle, ADC_CHANNEL_6, &config);

    while (1) {
        int adc_raw = read_adc_averaged(adc1_handle);
        float angle = adc_to_angle(adc_raw);

        printf("ADC = %d  Angle = %.1f deg\n", adc_raw, angle);

        vTaskDelay(pdMS_TO_TICKS(200));
    }
}