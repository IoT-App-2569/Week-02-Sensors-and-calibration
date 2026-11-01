# รายงานการทดลอง: การวัดมุมด้วย POT และ ESP32

## 1. วัตถุประสงค์
- เรียนรู้การใช้อนุพันธ์อ่านค่า ADC ของ ESP32 ผ่าน ESP-IDF
- สร้างและปรับแต่ง (Calibrate) ชุดวัดมุมด้วย Potentiometer (POT) ร่วมกับหน้าปัดกระดาษ
- แปลงค่าสัญญาณอนาล็อก (ADC) เป็นค่ามุมจริงด้วยสมการเชิงเส้น

## 2. อุปกรณ์และวงจรไฟฟ้า
- **อุปกรณ์:** ESP32, POT 10k, กระดาษหน้าปัดมุม ($0^\circ - 180^\circ$)
- **การต่อวงจร:**
  - ขาซ้าย POT $\rightarrow$ **3.3V**
  - ขาขวา POT $\rightarrow$ **GND**
  - ขากลาง POT $\rightarrow$ **GPIO34** (`ADC1_CH6`)

---

## 3. สรุปขั้นตอนและโค้ดการทดลอง (ESP-IDF)

### 3.1 อ่านค่า ADC เบื้องต้น
ใช้ `adc1_config_width(ADC_WIDTH_BIT_12)` และ `adc1_config_channel_atten(ADC1_CHANNEL_6, ADC_ATTEN_DB_11)` เพื่ออ่านค่าช่วง 0–4095

### 3.2 ข้อมูลการคาลิเบรต (Calibration Data)

$$angle = \frac{ADC - ADC_{min}}{ADC_{max} - ADC_{min}} \cdot \theta_{max}$$

*กำหนดให้:* $ADC_{min} = 180$, $ADC_{max} = 3900$, $\theta_{max} = 180^\circ$

| มุมจริง ($\theta$) | ค่า ADC คาดการณ์ | ค่า ADC ที่วัดได้จริง |
| :---: | :---: | :---: |
| $0^\circ$ | 180 | *(บันทึกจากการทดลอง)* |
| $45^\circ$ | 1100 | *(บันทึกจากการทดลอง)* |
| $90^\circ$ | 2100 | *(บันทึกจากการทดลอง)* |
| $135^\circ$ | 3000 | *(บันทึกจากการทดลอง)* |
| $180^\circ$ | 3900 | *(บันทึกจากการทดลอง)* |

---

### 3.3 โค้ดแสดงผลมุมแบบเรียลไทม์

```c
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"

#define ADC_MIN 180
#define ADC_MAX 3900
#define DEG_MAX 180.0f

float adc_to_angle(int adc) {
    if (adc < ADC_MIN) return 0.0f;
    if (adc > ADC_MAX) return DEG_MAX;
    return (adc - ADC_MIN) * DEG_MAX / (ADC_MAX - ADC_MIN);
}

void app_main(void) {
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ADC1_CHANNEL_6, ADC_ATTEN_DB_11);

    while (1) {
        int adc = adc1_get_raw(ADC1_CHANNEL_6);
        float angle = adc_to_angle(adc);
        printf("ADC: %d | Angle: %.2f deg\n", adc, angle);
        vTaskDelay(pdMS_TO_TICKS(300));
    }
}
```

---

## 4. การตรวจสอบและสรุปผล (Troubleshooting & Summary)

### การวิเคราะห์ความคลาดเคลื่อน (Error)
หากพบว่าค่ามุมที่อ่านได้มีความคลาดเคลื่อนสูง ให้ตรวจสอบดังนี้:
1. **Physical Alignment:** แกนหมุน POT ติดตั้งตรงกับจุดศูนย์กลางของหน้าปัดกระดาษหรือไม่
2. **Boundary Values:** ตรวจสอบและอัปเดตค่า `ADC_MIN` และ `ADC_MAX` ให้อ้างอิงจากฮาร์ดแวร์จริง
3. **Non-linearity Correction:** หาก ADC ของ ESP32 มีความไม่เป็นเชิงเส้น (Non-linear) ในช่วงแรงดันต่ำหรือสูง ให้เปลี่ยนไปใช้ **Piecewise Calibration** (แยกช่วงคำนวณทีละมุม) แทนสมการเส้นตรงเดียว