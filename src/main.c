/* USER CODE BEGIN 0 */
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
/* USER CODE END 0 */