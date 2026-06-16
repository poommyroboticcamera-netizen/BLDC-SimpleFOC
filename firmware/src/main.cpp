// src/main.cpp
#include "stm32f4xx_hal.h"
#include <math.h>
#include <stdbool.h>

TIM_HandleTypeDef htim1;

// H2 mapping from your schematic:
// H2-10 HIN_A -> PA8  TIM1_CH1
// H2-9  LIN_A -> PB13 TIM1_CH1N
// H2-8  HIN_B -> PA9  TIM1_CH2
// H2-7  LIN_B -> PB14 TIM1_CH2N
// H2-6  HIN_C -> PA10 TIM1_CH3
// H2-5  LIN_C -> PB15 TIM1_CH3N


static constexpr uint32_t PWM_FREQ_HZ = 20000;
static constexpr uint32_t TIM1_CLK_HZ = 168000000;
static constexpr uint32_t PWM_PERIOD = (TIM1_CLK_HZ / (2 * PWM_FREQ_HZ)) - 1;

// DTG 190 is about 1.5 us at TIM1 = 168 MHz. Verify on scope.
static constexpr uint32_t DEADTIME_DTG = 190;

static constexpr float TWO_PI = 6.28318530718f;
static constexpr float PHASE_120 = 2.09439510239f;

static constexpr float MIN_DUTY = 0.05f;
static constexpr float MAX_DUTY = 0.95f;
static constexpr float ALIGN_MODULATION = 0.035f;
static constexpr float RUN_MODULATION = 0.060f; /////////////////////////

static constexpr uint32_t ALIGN_TIME_MS = 1200;
static constexpr float START_ELECTRICAL_HZ = 0.8f; /////////////
static constexpr float TARGET_ELECTRICAL_HZ = 8.0f; ///////////////
static constexpr float RAMP_HZ_PER_SEC = 0.8f;  //////////////

static void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM1_Init(void);
extern "C" void Error_Handler(void);

static float clampf(float v, float lo, float hi) {
  if (v < lo) return lo;
  if (v > hi) return hi;
  return v;
}

static uint32_t dutyToCompare(float duty) {
  duty = clampf(duty, MIN_DUTY, MAX_DUTY);
  return (uint32_t)(duty * (float)(PWM_PERIOD + 1));
}

static void setDuty(float a, float b, float c) {
  __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, dutyToCompare(a));
  __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, dutyToCompare(b));
  __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, dutyToCompare(c));
}

static void setNeutralDuty(void) {
  setDuty(0.5f, 0.5f, 0.5f);
}

static void setOpenLoopSine(float theta, float modulation) {
  modulation = clampf(modulation, 0.0f, 0.12f);

  float da = 0.5f + modulation * sinf(theta);
  float db = 0.5f + modulation * sinf(theta - PHASE_120);
  float dc = 0.5f + modulation * sinf(theta + PHASE_120);

  setDuty(da, db, dc);
}

static void preparePwmOutputs(void) {
  setNeutralDuty();
  TIM1->EGR = TIM_EGR_UG;

  TIM1->CCER |= TIM_CCER_CC1E | TIM_CCER_CC1NE |
                TIM_CCER_CC2E | TIM_CCER_CC2NE |
                TIM_CCER_CC3E | TIM_CCER_CC3NE;

  TIM1->CR1 |= TIM_CR1_CEN;
  TIM1->BDTR &= ~TIM_BDTR_MOE;
}

static void bridgeArm(void) {
  setNeutralDuty();
  TIM1->EGR = TIM_EGR_UG;
  TIM1->BDTR |= TIM_BDTR_MOE;
}

static void bridgeDisarm(void) {
  TIM1->BDTR &= ~TIM_BDTR_MOE;
  setNeutralDuty();
}

int main(void) {
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_TIM1_Init();

  preparePwmOutputs();
  bridgeDisarm();

  // Safety boot delay: gives 12V/5V/3.3V rails time to settle.
  HAL_Delay(1500);

  bridgeArm();

  enum State { ALIGN, RUN };
  State state = ALIGN;

  float angle = 0.0f;
  uint32_t stateStart = HAL_GetTick();
  uint32_t lastTick = stateStart;

  while (1) {
    uint32_t now = HAL_GetTick();

    if (now == lastTick) {
      HAL_Delay(1);
      continue;
    }

    float dt = (float)(now - lastTick) * 0.001f;
    lastTick = now;

    if (state == ALIGN) {
      setOpenLoopSine(0.0f, ALIGN_MODULATION);

      if ((now - stateStart) >= ALIGN_TIME_MS) {
        state = RUN;
        stateStart = now;
        lastTick = now;
      }
    } else {
      float t = (float)(now - stateStart) * 0.001f;
      float hz = START_ELECTRICAL_HZ + (RAMP_HZ_PER_SEC * t);
      hz = clampf(hz, START_ELECTRICAL_HZ, TARGET_ELECTRICAL_HZ);

      angle += TWO_PI * hz * dt;
      while (angle >= TWO_PI) angle -= TWO_PI;

      setOpenLoopSine(angle, RUN_MODULATION);
    }
  }
}

static void MX_GPIO_Init(void) {
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  GPIO_InitTypeDef gpio = {};

  gpio.Pin = GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10;
  gpio.Mode = GPIO_MODE_AF_PP;
  gpio.Pull = GPIO_PULLDOWN;
  gpio.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  gpio.Alternate = GPIO_AF1_TIM1;
  HAL_GPIO_Init(GPIOA, &gpio);

  gpio.Pin = GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
  gpio.Mode = GPIO_MODE_AF_PP;
  gpio.Pull = GPIO_PULLDOWN;
  gpio.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  gpio.Alternate = GPIO_AF1_TIM1;
  HAL_GPIO_Init(GPIOB, &gpio);
}

static void MX_TIM1_Init(void) {
  __HAL_RCC_TIM1_CLK_ENABLE();

  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 0;
  htim1.Init.CounterMode = TIM_COUNTERMODE_CENTERALIGNED1;
  htim1.Init.Period = PWM_PERIOD;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;

  if (HAL_TIM_PWM_Init(&htim1) != HAL_OK) Error_Handler();

  TIM_OC_InitTypeDef oc = {};
  oc.OCMode = TIM_OCMODE_PWM1;
  oc.Pulse = (PWM_PERIOD + 1) / 2;
  oc.OCPolarity = TIM_OCPOLARITY_HIGH;
  oc.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  oc.OCFastMode = TIM_OCFAST_DISABLE;
  oc.OCIdleState = TIM_OCIDLESTATE_RESET;
  oc.OCNIdleState = TIM_OCNIDLESTATE_RESET;

  if (HAL_TIM_PWM_ConfigChannel(&htim1, &oc, TIM_CHANNEL_1) != HAL_OK) Error_Handler();
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &oc, TIM_CHANNEL_2) != HAL_OK) Error_Handler();
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &oc, TIM_CHANNEL_3) != HAL_OK) Error_Handler();

  TIM_BreakDeadTimeConfigTypeDef bd = {};
  bd.OffStateRunMode = TIM_OSSR_DISABLE;
  bd.OffStateIDLEMode = TIM_OSSI_ENABLE;
  bd.LockLevel = TIM_LOCKLEVEL_OFF;
  bd.DeadTime = DEADTIME_DTG;
  bd.BreakState = TIM_BREAK_DISABLE;
  bd.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  bd.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;

  if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &bd) != HAL_OK) Error_Handler();
}


static void SystemClock_Config(void) {
  RCC_OscInitTypeDef osc = {};
  RCC_ClkInitTypeDef clk = {};

  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  osc.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  osc.HSIState = RCC_HSI_ON;
  osc.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  osc.PLL.PLLState = RCC_PLL_ON;
  osc.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  osc.PLL.PLLM = 16;
  osc.PLL.PLLN = 336;
  osc.PLL.PLLP = RCC_PLLP_DIV2;
  osc.PLL.PLLQ = 7;

  if (HAL_RCC_OscConfig(&osc) != HAL_OK) Error_Handler();

  clk.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                  RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  clk.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  clk.AHBCLKDivider = RCC_SYSCLK_DIV1;
  clk.APB1CLKDivider = RCC_HCLK_DIV4;
  clk.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&clk, FLASH_LATENCY_5) != HAL_OK) Error_Handler();
}

extern "C" void Error_Handler(void) {
  __disable_irq();
  if ((RCC->APB2ENR & RCC_APB2ENR_TIM1EN) != 0U) {
    TIM1->BDTR &= ~TIM_BDTR_MOE;
  }
  while (1) {}
}