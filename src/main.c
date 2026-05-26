#include "main.h"

extern TIM_HandleTypeDef htim1;

// ---------------------------------------------------------
// ฟังก์ชันปิดทุกเฟส (สั่งรันเสมอเพื่อความปลอดภัย)
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
// ฟังก์ชัน 6-Step Commutation (อ้างอิงเฟส A, B, C ตามวงจร)
// ---------------------------------------------------------
void Set_Commutation_Step(uint8_t step, uint16_t speed_pwm) {
    Stop_All_Phases(); // ป้องกันไฟช็อตข้ามเฟสเสมอ

    switch (step) {
        case 1: 
            // เฟส A: PWM, เฟส B: Low, เฟส C: ลอย
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, speed_pwm); 
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 0);         
            
            HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
            HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1);
            HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
            HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2);
            break;

        case 2: 
            // เฟส A: PWM, เฟส B: ลอย, เฟส C: Low
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, speed_pwm); 
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, 0);         
            
            HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
            HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1);
            HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
            HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_3);
            break;

        case 3: 
            // เฟส A: ลอย, เฟส B: PWM, เฟส C: Low
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, speed_pwm); 
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, 0);         
            
            HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
            HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2);
            HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
            HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_3);
            break;

        case 4: 
            // เฟส A: Low, เฟส B: PWM, เฟส C: ลอย
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 0);         
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, speed_pwm); 
            
            HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
            HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1);
            HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
            HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2);
            break;

        case 5: 
            // เฟส A: Low, เฟส B: ลอย, เฟส C: PWM
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 0);         
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, speed_pwm); 
            
            HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
            HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1);
            HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
            HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_3);
            break;

        case 6: 
            // เฟส A: ลอย, เฟส B: Low, เฟส C: PWM
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
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_TIM1_Init();

    // สมมติว่า ARR คือ 8399 
    // ตั้ง Duty Cycle ไว้ที่ 10% เพื่อทดสอบสัญญาณ (10% ของ 8400 คือ 840)
    uint16_t motor_speed = 840; 
    uint8_t current_step = 1;

    while (1) {
        Set_Commutation_Step(current_step, motor_speed);
        
        current_step++;
        if (current_step > 6) {
            current_step = 1;
        }

        // ความเร็วในการเปลี่ยน Step สำหรับสโคปเช็คสัญญาณ
        HAL_Delay(10); 
    }
}