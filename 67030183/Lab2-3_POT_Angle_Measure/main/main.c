#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_log.h"

// ============================================================
//  ตั้งค่าระบบคาลิเบรต
//  - 7 จุด ทุก 30 องศา (0°, 30°, 60°, 90°, 120°, 150°, 180°)
//  - ยิ่งจุดถี่ ยิ่งแม่นยำ (Piecewise Interpolation)
// ============================================================
#define NUM_POINTS   5
#define CAL_SAMPLES  64   // จำนวนครั้งที่อ่านเฉลี่ยตอนคาลิเบรต
#define READ_SAMPLES 32   // จำนวนครั้งที่อ่านเฉลี่ยตอนวัดมุม

static adc_oneshot_unit_handle_t adc1_handle;

// ตารางคาลิเบรต (จะถูกเติมค่าจริงตอนบูต)
static int   cal_adc[NUM_POINTS];
static const float cal_deg[NUM_POINTS] = {0.0, 45.0, 90.0, 135.0, 180.0};

// ============================================================
//  อ่านค่า ADC แบบเฉลี่ย (Moving Average Filter)
// ============================================================
int read_adc_averaged(int samples)
{
    int sum = 0;
    for (int i = 0; i < samples; i++) {
        int raw = 0;
        adc_oneshot_read(adc1_handle, ADC_CHANNEL_6, &raw);
        sum += raw;
    }
    return sum / samples;
}

// ============================================================
//  แปลง ADC → มุม ด้วย Piecewise Linear Interpolation
//  (เทียบบัญญัติไตรยางศ์แยกทีละช่วง)
// ============================================================
float adc_to_angle(int adc)
{
    // ค่า ADC ลดลงเมื่อมุมเพิ่มขึ้น (เพราะการต่อสายของ POT)
    if (adc >= cal_adc[0])              return cal_deg[0];
    if (adc <= cal_adc[NUM_POINTS - 1]) return cal_deg[NUM_POINTS - 1];

    for (int i = 0; i < NUM_POINTS - 1; i++) {
        if (adc <= cal_adc[i] && adc >= cal_adc[i + 1]) {
            float ratio = (float)(adc - cal_adc[i])
                        / (float)(cal_adc[i + 1] - cal_adc[i]);
            return cal_deg[i] + ratio * (cal_deg[i + 1] - cal_deg[i]);
        }
    }
    return 0.0;
}

// ============================================================
//  โหมดคาลิเบรตแบบ Interactive
//  - แสดง ADC แบบ Live ให้ผู้ใช้ดูว่านิ่งหรือยัง
//  - กด Enter เพื่อบันทึกค่าแต่ละจุด
// ============================================================
void run_calibration(void)
{
    printf("\n");
    printf("================================================\n");
    printf("   CALIBRATION MODE  (%d จุด ทุก 30 องศา)\n", NUM_POINTS);
    printf("================================================\n");
    printf("  วิธีใช้:\n");
    printf("  1. หมุน POT ไปที่มุมบนกระดาษตามที่กำหนด\n");
    printf("  2. ดูค่า ADC ที่แสดง รอให้ค่านิ่ง\n");
    printf("  3. กด Enter เพื่อบันทึก\n");
    printf("================================================\n\n");

    // ลองตั้ง stdin เป็น non-blocking เพื่อแสดง ADC แบบ live
    int old_flags = fcntl(fileno(stdin), F_GETFL, 0);
    int nb_ok = fcntl(fileno(stdin), F_SETFL, old_flags | O_NONBLOCK);

    for (int i = 0; i < NUM_POINTS; i++) {
        printf("[%d/%d] >>> หมุน POT ไปที่ %3.0f° แล้วกด Enter\n",
               i + 1, NUM_POINTS, cal_deg[i]);

        if (nb_ok == 0) {
            // === โหมด Live: แสดง ADC แบบเรียลไทม์ รอ Enter ===
            while (1) {
                int live = read_adc_averaged(16);
                printf("\r  ADC = %4d  (กด Enter เพื่อบันทึก) ", live);
                fflush(stdout);

                int ch = fgetc(stdin);
                if (ch == '\n' || ch == '\r') break;

                vTaskDelay(pdMS_TO_TICKS(200));
            }
        } else {
            // === Fallback: blocking ===
            printf("  กด Enter เพื่อบันทึก: ");
            fflush(stdout);
            char buf[16];
            fgets(buf, sizeof(buf), stdin);
        }

        // บันทึกค่าเฉลี่ย 64 ครั้ง
        cal_adc[i] = read_adc_averaged(CAL_SAMPLES);
        printf("\n  -> บันทึกสำเร็จ: %3.0f° = ADC %4d\n\n", cal_deg[i], cal_adc[i]);
    }

    // คืนค่า stdin เป็น blocking
    if (nb_ok == 0) {
        fcntl(fileno(stdin), F_SETFL, old_flags);
    }

    // แสดงตารางสรุป
    printf("================================================\n");
    printf("   สรุปข้อมูลคาลิเบรต\n");
    printf("================================================\n");
    printf("   มุม (deg)  |  ADC\n");
    printf("  ------------+---------\n");
    for (int i = 0; i < NUM_POINTS; i++) {
        printf("   %5.0f      |  %4d\n", cal_deg[i], cal_adc[i]);
    }
    printf("================================================\n");
    printf("  คาลิเบรตเสร็จสิ้น! เริ่มวัดมุมแบบเรียลไทม์...\n");
    printf("  (รีเซ็ตบอร์ดเพื่อคาลิเบรตใหม่)\n");
    printf("================================================\n\n");
}

// ============================================================
//  app_main
// ============================================================
void app_main(void)
{
    // 1. ตั้งค่า ADC1 Channel 6 (GPIO34)
    adc_oneshot_unit_init_cfg_t init_cfg = {
        .unit_id = ADC_UNIT_1,
    };
    adc_oneshot_new_unit(&init_cfg, &adc1_handle);

    adc_oneshot_chan_cfg_t chan_cfg = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten   = ADC_ATTEN_DB_12,
    };
    adc_oneshot_config_channel(adc1_handle, ADC_CHANNEL_6, &chan_cfg);

    // 2. คาลิเบรตทุกครั้งที่บูต (เพื่อให้ตรงกับตำแหน่ง POT จริงปัจจุบัน)
    run_calibration();

    // 3. วัดมุมแบบเรียลไทม์
    while (1) {
        int adc   = read_adc_averaged(READ_SAMPLES);
        float ang = adc_to_angle(adc);
        printf("ADC = %4d  Angle = %6.2f deg\n", adc, ang);
        vTaskDelay(pdMS_TO_TICKS(300));
    }
}
