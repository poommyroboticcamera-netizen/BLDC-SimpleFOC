/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim1;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM1_Init(void);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
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
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/
  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM1_Init();

  /* USER CODE BEGIN 2 */

  // สมมติว่า ARR คือ 8399
  // ตั้ง Duty Cycle ไว้ที่ 10% เพื่อทดสอบสัญญาณ (10% ของ 8400 คือ 840)
  uint16_t motor_speed = 840;
  uint8_t current_step = 1;

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

    Set_Commutation_Step(current_step, motor_speed);

    current_step++;
    if (current_step > 6) {
        current_step = 1;
    }

    // หน่วงเวลา 10ms สำหรับสโคปเช็คสัญญาณ หรือให้มอเตอร์ขยับสเต็ป
    HAL_Delay(10);

  }
  /* USER CODE END 3 */
}

/* (โค้ดส่วนล่างๆ ที่เป็น SystemClock_Config และ MX_TIM1_Init ปล่อยไว้ตามเดิมได้เลยครับ) */
