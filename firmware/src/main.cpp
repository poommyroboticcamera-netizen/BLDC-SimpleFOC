#include <Arduino.h>
#include <STM32FreeRTOS.h>
#include <math.h>

// ---------------------------------------------------------
// 1. การตั้งค่าพินฮาร์ดแวร์ (Custom MKS 75100 + EG3112)
// ---------------------------------------------------------
// ขาอ่านแรงดันเฟส (ADC Phase Voltage Sensing)
#define PA_V_PIN PA0 
#define PB_V_PIN PA1
#define PC_V_PIN PA2

// พารามิเตอร์ฮาร์ดแวร์ระดับล่าง
#define HW_DEAD_TIME_NSEC   360.0f  // เวลา Dead-time 360 ns
#define PWM_FREQ_HZ         20000   // ความถี่ PWM 20 kHz

const float PI_3 = 3.14159265359f;
uint32_t timer_top; 

// ---------------------------------------------------------
// 2. ตัวแปรควบคุมมอเตอร์ (ปรับเปลี่ยนผ่าน Serial ได้)
// ---------------------------------------------------------
float electrical_angle = 0.0f;
float rotation_speed = 1.0f;      // ความเร็วสนามแม่เหล็กเริ่มต้น (องศา/ลูป)
float duty_percent = 5.0f;        // ความแรงเริ่มต้นที่ 5% (เพื่อความปลอดภัย)

// ค่าตัวต้านทาน Voltage Divider บนบอร์ด
const float R1_PHASE = 39000.0f; 
const float R2_PHASE = 2200.0f;

// ---------------------------------------------------------
// 3. ฟังก์ชันตั้งค่าฮาร์ดแวร์ TIM1 (STM32F4 Bare-metal)
// ---------------------------------------------------------
void init_vesc_hardware() {
  // เปิด Clock ให้พอร์ต A, B และ TIM1
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN;
  RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;

  // เซ็ตขา PA8, PA9, PA10 ให้เป็น Alternate Function 1 (TIM1_CH1..3 High-side)
  GPIOA->MODER |= GPIO_MODER_MODER8_1 | GPIO_MODER_MODER9_1 | GPIO_MODER_MODER10_1;
  GPIOA->AFR[1] |= (1 << 0) | (1 << 4) | (1 << 8); 
  
  // เซ็ตขา PB13, PB14, PB15 ให้เป็น Alternate Function 1 (TIM1_CH1N..3N Low-side)
  GPIOB->MODER |= GPIO_MODER_MODER13_1 | GPIO_MODER_MODER14_1 | GPIO_MODER_MODER15_1;
  GPIOB->AFR[1] |= (1 << 20) | (1 << 24) | (1 << 28); 

  // ตั้งค่า TIM1 แบบ Center-aligned
  timer_top = SystemCoreClock / (PWM_FREQ_HZ * 2); 
  TIM1->CR1 = 0;
  TIM1->ARR = timer_top; 
  TIM1->PSC = 0;         
  TIM1->CR1 |= TIM_CR1_CMS_0 | TIM_CR1_CMS_1; 

  // ตั้งโหมด PWM1 
  TIM1->CCMR1 = TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1PE |
                TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2PE;
  TIM1->CCMR2 = TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3M_2 | TIM_CCMR2_OC3PE;

  // เปิดใช้งาน Complementary outputs (สลับบน-ล่าง)
  TIM1->CCER = TIM_CCER_CC1E | TIM_CCER_CC1NE |
               TIM_CCER_CC2E | TIM_CCER_CC2NE |
               TIM_CCER_CC3E | TIM_CCER_CC3NE;

  // แทรก Dead-time ลงระดับฮาร์ดแวร์
  uint32_t dt_ticks = (uint32_t)((HW_DEAD_TIME_NSEC * SystemCoreClock) / 1000000000.0f);
  if (dt_ticks > 127) dt_ticks = 127; 
  TIM1->BDTR = TIM_BDTR_MOE | (dt_ticks & TIM_BDTR_DTG);

  // เริ่มรัน Timer
  TIM1->CR1 |= TIM_CR1_CEN;
}

// ---------------------------------------------------------
// 4. FreeRTOS Tasks
// ---------------------------------------------------------

// Task 1: อัปเดตลูป 6-PWM แบบ Open-loop (ทำงานแม่นยำทุก 1ms)
void TaskMotorControl(void *pvParameters) {
  (void) pvParameters;
  TickType_t xLastWakeTime = xTaskGetTickCount();
  const TickType_t xFrequency = pdMS_TO_TICKS(1); 

  for (;;) {
    electrical_angle += rotation_speed;
    if (electrical_angle >= 360.0f) electrical_angle -= 360.0f;

    // แปลง Duty 0-100% ให้เป็นสเกล Amplitude (0.0 ถึง 0.5) ของคลื่น AC
    float amplitude = (duty_percent / 100.0f) / 2.0f;
    if (amplitude > 0.5f) amplitude = 0.5f; 

    // คำนวณเฟส
    float radA = electrical_angle * (PI_3 / 180.0f);
    float radB = (electrical_angle + 120.0f) * (PI_3 / 180.0f);
    float radC = (electrical_angle + 240.0f) * (PI_3 / 180.0f);

    // เลื่อนศูนย์กลางคลื่นไปที่ 0.5 (ทำงานแบบ Symmetrical)
    float valA = 0.5f + (amplitude * sin(radA));
    float valB = 0.5f + (amplitude * sin(radB));
    float valC = 0.5f + (amplitude * sin(radC));

    // อัปเดต Register โดยตรง
    TIM1->CCR1 = (uint32_t)(valA * timer_top);
    TIM1->CCR2 = (uint32_t)(valB * timer_top);
    TIM1->CCR3 = (uint32_t)(valC * timer_top);

    vTaskDelayUntil(&xLastWakeTime, xFrequency); 
  }
}

// Task 2: รับคำสั่ง Serial และอ่านแรงดันเฟส ADC (ทำงานทุก 200ms)
void TaskMonitor(void *pvParameters) {
  (void) pvParameters;
  analogReadResolution(12); // ตั้ง ADC เป็น 12-bit (0-4095)

  for (;;) {
    // --- รับคำสั่งปรับ Duty (พิมพ์ D) หรือ Speed (พิมพ์ S) ---
    if (Serial.available() > 0) {
      char command = Serial.read(); 
      float value = Serial.parseFloat(); 

      if (command == 'D' || command == 'd') {
        if (value >= 0.0f && value <= 100.0f) {
          duty_percent = value;
          Serial.print("\n>>> DUTY CYCLE SET TO: ");
          Serial.print(duty_percent);
          Serial.println("% <<<\n");
        }
      } 
      else if (command == 'S' || command == 's') {
        if (value >= 0.0f && value <= 20.0f) { 
          rotation_speed = value;
          Serial.print("\n>>> ROTATION SPEED SET TO: ");
          Serial.print(rotation_speed);
          Serial.println(" Deg/Loop <<<\n");
        }
      }
    }

    // --- อ่านและคำนวณแรงดันเฟส ---
    int adc_pa = analogRead(PA_V_PIN);
    int adc_pb = analogRead(PB_V_PIN);
    int adc_pc = analogRead(PC_V_PIN);

    float ratio = (R1_PHASE + R2_PHASE) / R2_PHASE;
    float v_pa = (adc_pa / 4095.0f) * 3.3f * ratio;
    float v_pb = (adc_pb / 4095.0f) * 3.3f * ratio;
    float v_pc = (adc_pc / 4095.0f) * 3.3f * ratio;

    // พิมพ์ผลลัพธ์
    Serial.print("Cmd Duty: "); Serial.print(duty_percent); 
    Serial.print("% | Speed: "); Serial.print(rotation_speed); Serial.print(" | ");
    Serial.print("VA: "); Serial.print(v_pa);
    Serial.print("V | VB: "); Serial.print(v_pb);
    Serial.print("V | VC: "); Serial.print(v_pc);
    Serial.println("V");

    vTaskDelay(pdMS_TO_TICKS(200));
  }
}

// ---------------------------------------------------------
// 5. Main Setup
// ---------------------------------------------------------
void setup() {
  Serial.begin(115200);
  
  // ตั้งค่าฮาร์ดแวร์ 
  init_vesc_hardware();
  
  Serial.println("=========================================");
  Serial.println("   Custom MKS 75100 V/f Control Ready    ");
  Serial.println("=========================================");
  Serial.println("Commands:");
  Serial.println("  D<value> : Set Duty Cycle (0-100%) e.g. D10");
  Serial.println("  S<value> : Set Speed (Deg/Loop)    e.g. S2.5");
  Serial.println("-----------------------------------------");

  // สร้าง Tasks ในระบบ FreeRTOS
  xTaskCreate(TaskMotorControl, "MotorCtrl", 512, NULL, 2, NULL); // Priority สูงกว่า
  xTaskCreate(TaskMonitor, "Monitor", 256, NULL, 1, NULL);        // Priority ต่ำกว่า

  // สตาร์ทตัวจัดการ RTOS
  vTaskStartScheduler();
}

void loop() {
  // FreeRTOS จัดการให้หมดแล้ว
}

