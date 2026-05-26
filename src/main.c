#include "main.h"

// กำหนดตัวแปร Timer 1 ที่ถูกตั้งค่ามาจากฟังก์ชัน MX_TIM1_Init()
extern TIM_HandleTypeDef htim1;

// ---------------------------------------------------------
// ฟังก์ชันสำหรับปิดทุกเฟส (ใช้เพื่อความปลอดภัยตอนเริ่มหรือหยุดมอเตอร์)
// ---------------------------------------------------------
void Stop_All_Phases(void) {
    HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
    HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_1);
    
    HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_2);
    HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_2);
    
    HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_3);
    HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_3);
}

// ---------------------------------------------------------
// ฟังก์ชันสำหรับสลับเฟส 6 จังหวะตามตำแหน่งโรเตอร์
// ---------------------------------------------------------
void Set_Commutation_Step(uint8_t step, uint16_t speed_pwm) {
    // 1. ปิดทุกเฟสก่อนเสมอ เพื่อป้องกันการช็อตข้ามเฟสขณะสลับสถานะ
    Stop_All_Phases();

    // 2. สลับการทำงานตาม Step 1 ถึง 6
    // หลักการ: จ่าย PWM เพื่อดันกระแส, ดึง Low (Duty 0) ลงกราวด์, และ Stop เพื่อปล่อยลอย (ปิดสนิท) 
    switch (step) {
        case 1: 
            // เฟส U: PWM, เฟส V: Low, เฟส W: ปิดสนิท(ลอย) [cite: 13]
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, speed_pwm); 
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 0);         
            
            HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
            HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1);
            HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
            HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2);
            break;

        case 2: 
            // เฟส U: PWM, เฟส V: ปิดสนิท(ลอย), เฟส W: Low [cite: 14]
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, speed_pwm); 
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, 0);         
            
            HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
            HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1);
            HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
            HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_3);
            break;

        case 3: 
            // เฟส U: ปิดสนิท(ลอย), เฟส V: PWM, เฟส W: Low [cite: 14]
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, speed_pwm); 
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, 0);         
            
            HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
            HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2);
            HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
            HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_3);
            break;

        case 4: 
            // เฟส U: Low, เฟส V: PWM, เฟส W: ปิดสนิท(ลอย) [cite: 15]
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 0);         
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, speed_pwm); 
            
            HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
            HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1);
            HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
            HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2);
            break;

        case 5: 
            // เฟส U: Low, เฟส V: ปิดสนิท(ลอย), เฟส W: PWM [cite: 15]
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 0);         
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, speed_pwm); 
            
            HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
            HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1);
            HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
            HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_3);
            break;

        case 6: 
            // เฟส U: ปิดสนิท(ลอย), เฟส V: Low, เฟส W: PWM [cite: 16]
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 0);         
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, speed_pwm); 
            
            HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
            HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2);
            HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
            HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_3);
            break;
    }
}

// ---------------------------------------------------------
// ฟังก์ชัน Main
// ---------------------------------------------------------
int main(void) {
    // โค้ดส่วนนี้มักจะถูกสร้างโดยอัตโนมัติจาก STM32CubeMX
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_TIM1_Init();

    // สมมติว่า Counter Period (ARR) ของ Timer 1 คือ 3599
    // กำหนด Duty Cycle เริ่มต้นเพื่อทดสอบสัญญาณ 
    uint16_t motor_speed = 360; 
    uint8_t current_step = 1;

    while (1) {
        // ทดสอบรันลูปสลับเฟส 1 ถึง 6 แบบ Open-loop
        Set_Commutation_Step(current_step, motor_speed);
        
        current_step++;
        if (current_step > 6) {
            current_step = 1;
        }

        // หน่วงเวลาเพื่อให้จับสัญญาณสโคปได้ หรือให้มอเตอร์ค่อยๆ สลับสเต็ป
        HAL_Delay(10); 
    }
}