# Week 02: Analog Sensors, ADC Interfacing & Potentiometer Calibration

## 1. บทนำ (Introduction)
ในสัปดาห์ที่ 1 เราได้เรียนรู้ภาพรวมสถาปัตยกรรม IoT ทั้ง 7 ชั้น (IoTWF 7-Layer Model) และเข้าใจวงจรเชื่อมต่อโลกกายภาพสู่ดิจิทัล (Sense $\rightarrow$ Process $\rightarrow$ Actuate)

ในสัปดาห์ที่ 2 นี้ เราจะเจาะลึกลงสู่ **ชั้นที่ 1: Physical Devices & Controllers** โดยเน้นการทำงานของ **เซนเซอร์และทรานสดิวเซอร์แบบอนาล็อก (Analog Sensors & Transducers)** และการแปลงสัญญาณทางกายภาพให้อยู่ในรูปข้อมูลดิจิทัลผ่าน **โมดูลแปลงสัญญาณอนาล็อกเป็นดิจิทัล (Analog-to-Digital Converter: ADC)** ของไมโครคอนโทรลเลอร์ **ESP32** ภายใต้เฟรมเวิร์กมาตรฐานระดับอุตสาหกรรม **ESP-IDF (C / FreeRTOS)**

หัวใจสำคัญของการทำงานกับสัญญาณอนาล็อกในงานวิศวกรรม IoT คือ **"ความแม่นยำและการเทียบมาตรฐาน (Calibration & Accuracy)"** เนื่องจากสัญญาณกายภาพในโลกจริง (เช่น มุมการหมุน, ความเข้มแสง, แรงดัน) มีลักษณะต่อเนื่องและแปรผันตามคุณสมบัติทางไฟฟ้า แต่วงจร ADC ภายในไมโครคอนโทรลเลอร์มักมีข้อจำกัด เช่น ความคลาดเคลื่อนชดเชย (Offset Error), ความไม่เป็นเชิงเส้น (Non-linearity), และช่วงความคลาดเคลื่อนที่ส่วนขอบของแรงดัน (Deadband) นักศึกษาจึงจำเป็นต้องเข้าใจทั้งเชิงฮาร์ดแวร์ การจัดการโปรแกรม และคณิตศาสตร์การคาลิเบรต:

1. **Transducer & Voltage Divider Principle**: ศึกษาการแปลงปรากฏการณ์ทางกายภาพ (การหมุนมุม) เป็นความต้านทาน และแปลงเป็นแรงดันไฟฟ้า $0 - 3.3\text{ V}$ ด้วยวงจรแบ่งแรงดัน (Voltage Divider) ผ่านตัวต้านทานปรับค่าได้ (Potentiometer)
2. **ESP32 ADC Architecture & Attenuation**: สถาปัตยกรรม Successive Approximation Register (SAR) ADC ขนาด 12-bit (ให้ค่า $0 - 4095$) และการเลือกใช้อัตราลดทอนสัญญาณ (Attenuation) ที่เหมาะสมด้วย `driver/adc.h`
3. **ESP-IDF Project Development From Scratch**: โครงสร้างโฟลเดอร์โปรเจกต์ตามมาตรฐาน ESP-IDF, ไฟล์ `CMakeLists.txt` ระดับ Root และ Component, ตลอดจนกระบวนการคอมไพล์และอัปโหลด (`idf.py build flash monitor`)
4. **Sensor Calibration & Transfer Function**: ระเบียบวิธีการเก็บข้อมูลอ้างอิง (Ground Truth Data Collection) ร่วมกับหน้าปัดกระดาษวัดมุม และการสร้างสมการแปลงค่าเชิงเส้น (Two-Point Linear Calibration) จากค่า Raw ADC ไปสู่หน่วยองศาจริง
5. **Measurement Verification & Error Forensics**: การทดสอบความถูกต้องของระบบวัดมุม การประเมินค่าความคลาดเคลื่อน (Absolute / Relative Error) และแนวทางแก้ปัญหาความไม่เป็นเชิงเส้นด้วย Piecewise Linear Calibration

* เอกสารประกอบการเรียนรู้และคู่มืออ้างอิง:
  * **Espressif ESP-IDF Programming Guide**: [On-Chip Analog to Digital Converter (ADC)](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/adc_oneshot.html)
  * เอกสารคำอธิบายรายวิชา: [Week-02-Analog-Sensors-and-Transducers.md](Week-02-Analog-Sensors-and-Transducers.md)
  * คู่มือการสร้างโปรเจกต์: [LabSheet/ESP32 from scrash.md](LabSheet/ESP32%20from%20scrash.md)

---

## 2. แผนผังเนื้อหาการเรียนรู้ประจำสัปดาห์ (Lesson Roadmap)

![](Fig_README_2026-10-27T07-58-49_academic.svg)

---

## 3. ตารางเปรียบเทียบระดับการลดทอนสัญญาณของ ESP32 ADC (ADC Attenuation Matrix)

วงจร ADC ภายใน ESP32 ไม่สามารถรับแรงดันสูงเต็ม $3.3\text{ V}$ ได้โดยตรงที่ระดับสัญญาณเดิม จึงต้องผ่านวงจรลดทอนสัญญาณ (Attenuation) เพื่อขยายช่วงแรงดันที่วัดได้ (Full-Scale Voltage):

| ค่าคอนฟิก Attenuation | ระดับลดทอน (dB) | ช่วงแรงดันขาเข้าที่แนะนำ ($V_{in}$) | ค่าสเกล Raw ADC (12-bit) | ลักษณะการนำไปประยุกต์ใช้ |
| :--- | :---: | :---: | :---: | :--- |
| **`ADC_ATTEN_DB_0`** | 0 dB | $100\text{ mV} - 950\text{ mV}$ | $0 - 4095$ | สัญญาณขนาดเล็กมาก เช่น Thermocouple หรือเซนเซอร์ระดับไมโครโวลต์ |
| **`ADC_ATTEN_DB_2_5`** | 2.5 dB | $100\text{ mV} - 1250\text{ mV}$ | $0 - 4095$ | เซนเซอร์แรงดันต่ำ เช่น เซนเซอร์วัดสัญญาณชีวภาพ (Biomedical) |
| **`ADC_ATTEN_DB_6`** | 6 dB | $150\text{ mV} - 1750\text{ mV}$ | $0 - 4095$ | สัญญาณแรงดันระบบ $1.5\text{ V} - 1.8\text{ V}$ |
| **`ADC_ATTEN_DB_11`** | 11 dB | $150\text{ mV} - 2450\text{ mV}$<br>*(วัดได้ถึง $\sim 3.1\text{ V}$)* | $0 - 4095$ | **Potentiometer 10k, LDR, เซนเซอร์ Rail-to-Rail 3.3V (ที่ใช้ในการทดลองนี้)** |

> [!NOTE]
> **ข้อจำกัดเชิงฮาร์ดแวร์ของ ESP32 ADC:**
> 1. **Deadband ช่วงต่ำ ($< 100\text{ mV}$)**: แรงดันต่ำกว่า $0.1\text{ V}$ มักให้ค่าอ่านได้เป็น $0$ (ไม่สามารถแยกแยะได้)
> 2. **Non-linearity ช่วงสูง ($> 3.1\text{ V}$)**: ค่าที่อ่านได้จะเริ่มอิ่มตัว (Saturation) ก่อนถึง $3.3\text{ V}$ จึงจำเป็นต้องทำการเก็บค่า $ADC_{min}$ และ $ADC_{max}$ จริงหน้างานเสมอ

---

## 4. ตารางการต่อสายฮาร์ดแวร์และวงจรแบ่งแรงดัน (Hardware Wiring & Circuit Diagram)

การทดลองใช้ตัวต้านทานปรับค่าได้ชนิดเชิงเส้น (Linear Potentiometer 10k $\Omega$) ทำหน้าที่เป็นตัวแปรมุมเชิงกลให้เป็นแรงดันไฟฟ้าส่งเข้าช่องสัญญาณ **ADC1 Channel 6 (GPIO 34)** ของ ESP32:

| อุปกรณ์ | ขาบนตัวอุปกรณ์ | ขาต่อบนบอร์ด ESP32 | หน้าที่การทำงาน |
| :--- | :--- | :--- | :--- |
| **Potentiometer (10k)** | ขา 1 (ขาซ้าย / Terminal 1) | **3.3V** | แรงดันไฟเลี้ยงอ้างอิงบวก (Positive Reference Voltage) |
| **Potentiometer (10k)** | ขา 2 (ขากลาง / Wiper) | **GPIO 34** (ADC1_CH6) | สัญญาณแรงดันแปรผันตามมุมการหมุน ($V_{out}$) |
| **Potentiometer (10k)** | ขา 3 (ขาขวา / Terminal 2) | **GND** | กราวด์อ้างอิงระบบ (System Ground) |

```
         ESP32 Development Board                       10k Potentiometer
       +-------------------------+                   +-------------------+
       |                    3.3V |-------------------> Pin 1 (Terminal 1)|
       |   GPIO 34 (ADC1_CH6)    |<------------------- Pin 2 (Wiper/Out) |
       |                     GND |-------------------> Pin 3 Terminal 2) |
       +-------------------------+                   +-------------------+
                                                               |
                                                     [ Physical Paper Dial ]
                                                     0° ----- 90° ----- 180°
```

### สมการคาลิเบรตเชิงเส้น (Linear Calibration Equation)

เมื่อทำการบันทึกค่าที่ตำแหน่งต่ำสุด ($0^\circ$) เป็น $ADC_{min}$ และตำแหน่งสูงสุด ($\theta_{max}$) เป็น $ADC_{max}$ จะได้สมการแปลงค่า:

$$\text{Angle (Degree)} = \frac{ADC_{\text{raw}} - ADC_{min}}{ADC_{max} - ADC_{min}} \times \theta_{max}$$

---

## 5. รายการเอกสารบทเรียนและใบงานประจำสัปดาห์

1. **[Week-02-Analog-Sensors-and-Transducers.md](Week-02-Analog-Sensors-and-Transducers.md)** - สรุปกรอบเนื้อหาทฤษฎีและปฏิบัติการเซ็นเซอร์อนาล็อกประจำสัปดาห์ที่ 2
2. **[LabSheet/ESP32 from scrash.md](LabSheet/ESP32%20from%20scrash.md)** - คู่มือการสร้างโปรเจกต์ ESP32 From Scratch ด้วย ESP-IDF, อธิบายโครงสร้าง CMakeLists.txt และการใช้คำสั่ง `idf.py build flash monitor`
3. **[LabSheet/Gitignore for ESP-idf.md](LabSheet/Gitignore%20for%20ESP-idf.md)** - การตั้งค่าไฟล์ `.gitignore` มาตรฐานสำหรับโปรเจกต์ ESP-IDF เพื่อป้องกันการ push โฟลเดอร์ build และไฟล์ binary ขนาดใหญ่ขึ้น Git
4. **[LabSheet/Lab -- POT Angle Calibration Th.md](LabSheet/Lab%20--%20POT%20Angle%20Calibration%20Th.md)** - **ใบงานปฏิบัติการ: การทดลองวัดมุมด้วย Potentiometer ร่วมกับหน้าปัดกระดาษ และการคาลิเบรตค่าด้วย ESP32 (ฉบับภาษาไทย)**
5. **[LabSheet/Lab -- POT Angle Calibration En.md](LabSheet/Lab%20--%20POT%20Angle%20Calibration%20En.md)** - **Laboratory Sheet: Potentiometer Angle Calibration & Real-time Measurement on ESP32 (English Version)**

---

## 6. เครื่องมือและอุปกรณ์ที่จำเป็น (Prerequisites & Equipment)

* **ฮาร์ดแวร์สำหรับการทดลอง**:
  * บอร์ดไมโครคอนโทรลเลอร์ **ESP32** (Classic ESP32 หรือ ESP32-C3) จำนวน 1 บอร์ด
  * ตัวต้านทานปรับค่าได้ **Potentiometer 10k $\Omega$** (ชนิดแกนหมุน Linear B) จำนวน 1 ตัว
  * สายต่อวงจร (Jumper Wires) และเบรดบอร์ด (Breadboard)
  * กระดาษขาวหรือกระดาษแข็ง A4, ไม้โปรแทรกเตอร์วัดมุม, ปากกา, ไม้บรรทัด, และเทปกาวสำหรับสร้างหน้าปัดวัดมุม
* **สภาพแวดล้อมทางซอฟต์แวร์และการพัฒนา**:
  * ชุดเครื่องมือพัฒนา **ESP-IDF v5.x / v6.x** ติดตั้งพร้อมใช้งาน
  * โปรแกรม **Visual Studio Code** หรือ Command Line Terminal สำหรับการรันคำสั่ง `idf.py`
  * ระบบควบคุมเวอร์ชัน **Git** พร้อมการกำหนดไฟล์ `.gitignore` ที่ถูกต้อง
