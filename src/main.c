#include "main.h"

extern TIM_HandleTypeDef htim1;

// ฟังก์ชันสำหรับปิดทุกเฟส (ใช้เพื่อความปลอดภัยตอนเริ่มหรือหยุดมอเตอร์)
void Stop_All_Phases(void) {
    HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
    HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_1);
    
    HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_2);
    HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_2);
    
    HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_3);
    HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_3);
}

// ฟังก์ชันสำหรับสลับเฟส 6 จังหวะ
void Set_Commutation_Step(uint8_t step, uint16_t speed_pwm) {
    // 1. ปิดทุกเฟสก่อนเสมอ เพื่อป้องกันการช็อตข้ามเฟส (Shoot-through) ขณะสลับสถานะ
    Stop_All_Phases();

    // 2. สลับการทำงานตาม Step 1 ถึง 6
    switch (step) {
        case 1: 
            // เฟส U: PWM, เฟส V: Low, เฟส W: ปิดสนิท(ลอย)
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, speed_pwm); // U = PWM
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 0);         // V = Low (Duty 0%)
            
            HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
            HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1);
            HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
            HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2);
            break;

        case 2: 
            // เฟส U: PWM, เฟส V: ปิดสนิท(ลอย), เฟส W: Low
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, speed_pwm); // U = PWM
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, 0);         // W = Low
            
            HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
            HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1);
            HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
            HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_3);
            break;

        case 3: 
            // เฟส U: ปิดสนิท(ลอย), เฟส V: PWM, เฟส W: Low
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, speed_pwm); // V = PWM
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, 0);         // W = Low
            
            HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
            HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2);
            HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
            HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_3);
            break;

        case 4: 
            // เฟส U: Low, เฟส V: PWM, เฟส W: ปิดสนิท(ลอย)
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 0);         // U = Low
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, speed_pwm); // V = PWM
            
            HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
            HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1);
            HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
            HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2);
            break;

        case 5: 
            // เฟส U: Low, เฟส V: ปิดสนิท(ลอย), เฟส W: PWM
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 0);         // U = Low
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, speed_pwm); // W = PWM
            
            HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
            HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1);
            HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
            HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_3);
            break;

        case 6: 
            // เฟส U: ปิดสนิท(ลอย), เฟส V: Low, เฟส W: PWM
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 0);         // V = Low
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, speed_pwm); // W = PWM
            
            HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
            HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2);
            HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
            HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_3);
            break;
    }
}

int main(void) {
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();    MX_TIM1_Init();

    uint16_t motor_speed = 360; // กำหนดความเร็ว PWM (สมมติ ARR=3599 ค่านี้คือ Duty Cycle ประมาณ 10%)
    uint8_t current_step = 1;

    while (1) {
        // วนลูปสลับเฟส 1 ถึง 6 (Open-loop เพื่อทดสอบสัญญาณ)
        Set_Commutation_Step(current_step, motor_speed);
        
        current_step++;
        if (current_step > 6) {
            current_step = 1;
        }

        HAL_Delay(10); // หน่วงเวลาเพื่อให้สโคปจับสัญญาณได้ชัดเจน หรือให้มอเตอร์ค่อยๆ ขยับ
    }
}