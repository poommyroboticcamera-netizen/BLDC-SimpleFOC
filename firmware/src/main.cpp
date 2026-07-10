#include <Arduino.h>
#include <math.h>

/* ================= CONFIG ================= */
static const float BUS_VOLTAGE = 24.0f;
static const uint32_t PWM_FREQ_HZ = 20000;
static const uint32_t TIMER_CLOCK_HZ = 168000000;

static const uint8_t pole_pairs = 6;

/* motor control */
static float targetMechanicalRpm = -300.0f;
static float currentMechanicalRpm = 0.0f;
static float rampRpmPerSecond = 80.0f;

static float electricalAngle = 0.0f;
static float electricalRps = 0.0f;

/* control */
static float targetUqVolts = 2.5f;
static float startBoost = 2.8f;

/* TIM */
static uint16_t tim1Arr = 0;

/* ================= UTILS ================= */
static float clampf(float x, float a, float b)
{
    if (x < a) return a;
    if (x > b) return b;
    return x;
}

/* ================= DEADTIME ================= */
static uint8_t deadtimeTicksFromUs(float us)
{
    uint32_t ticks = (uint32_t)(us * TIMER_CLOCK_HZ / 1e6f);

    if (ticks <= 127) return (uint8_t)ticks;

    ticks >>= 1;
    if (ticks > 63) ticks = 63;

    return (uint8_t)(0x80 | ticks);
}

/* ================= GPIO + TIM1 ================= */
static void setupPWM()
{
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN;
    RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;

    /* PA8 PA9 PA10 = TIM1_CH1 CH2 CH3 */
    GPIOA->MODER &= ~((3 << (8 * 2)) | (3 << (9 * 2)) | (3 << (10 * 2)));
    GPIOA->MODER |=  ((2 << (8 * 2)) | (2 << (9 * 2)) | (2 << (10 * 2)));

    /* PB13 PB14 PB15 = TIM1_CH1N CH2N CH3N */
    GPIOB->MODER &= ~((3 << (13 * 2)) | (3 << (14 * 2)) | (3 << (15 * 2)));
    GPIOB->MODER |=  ((2 << (13 * 2)) | (2 << (14 * 2)) | (2 << (15 * 2)));

    /* AF1 = TIM1 */
    GPIOA->AFR[1] &= ~((0xF << 0) | (0xF << 4) | (0xF << 8));
    GPIOA->AFR[1] |=  ((1 << 0) | (1 << 4) | (1 << 8));

    GPIOB->AFR[1] &= ~((0xF << 20) | (0xF << 24) | (0xF << 28));
    GPIOB->AFR[1] |=  ((1 << 20) | (1 << 24) | (1 << 28));

    /* TIM1 base */
    TIM1->CR1 = 0;
    TIM1->PSC = 0;

    tim1Arr = (uint16_t)((TIMER_CLOCK_HZ / (2 * PWM_FREQ_HZ)) - 1);
    TIM1->ARR = tim1Arr;

    /* PWM mode 1 on CH1 CH2 CH3 */
    TIM1->CCMR1 =
        TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2 |
        TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2M_2;

    TIM1->CCMR2 =
        TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3M_2;

    /* Enable high-side and low-side outputs */
    TIM1->CCER =
        TIM_CCER_CC1E | TIM_CCER_CC1NE |
        TIM_CCER_CC2E | TIM_CCER_CC2NE |
        TIM_CCER_CC3E | TIM_CCER_CC3NE;

    /* Deadtime + main output enable */
    uint8_t dt = deadtimeTicksFromUs(1.0f);

    TIM1->BDTR =
        TIM_BDTR_MOE |
        TIM_BDTR_OSSR |
        TIM_BDTR_OSSI |
        dt;

    /* Center-aligned mode + preload */
    TIM1->CR1 = TIM_CR1_CMS_0 | TIM_CR1_ARPE;

    TIM1->EGR = TIM_EGR_UG;

    TIM1->CCR1 = tim1Arr / 2;
    TIM1->CCR2 = tim1Arr / 2;
    TIM1->CCR3 = tim1Arr / 2;

    TIM1->CR1 |= TIM_CR1_CEN;
}

/* ================= PWM WRITE ================= */
static void writeDuty(float da, float db, float dc)
{
    da = clampf(da, 0.02f, 0.98f);
    db = clampf(db, 0.02f, 0.98f);
    dc = clampf(dc, 0.02f, 0.98f);

    uint16_t top = tim1Arr + 1;

    TIM1->CCR1 = (uint16_t)(da * top);
    TIM1->CCR2 = (uint16_t)(db * top);
    TIM1->CCR3 = (uint16_t)(dc * top);
}

/* ================= SVPWM ================= */
static void setSVPWM(float alpha, float beta)
{
    float v1 = beta;
    float v2 = -0.5f * beta + 0.8660254f * alpha;
    float v3 = -0.5f * beta - 0.8660254f * alpha;

    float vmax = fmaxf(v1, fmaxf(v2, v3));
    float vmin = fminf(v1, fminf(v2, v3));

    float offset = 0.5f * (vmax + vmin);

    v1 -= offset;
    v2 -= offset;
    v3 -= offset;

    float da = 0.5f + (v1 / BUS_VOLTAGE);
    float db = 0.5f + (v2 / BUS_VOLTAGE);
    float dc = 0.5f + (v3 / BUS_VOLTAGE);

    writeDuty(da, db, dc);
}

/* ================= OPEN LOOP FOC ================= */
static void openLoopFOC(float theta, float uq)
{
    float alpha = -uq * sinf(theta);
    float beta  =  uq * cosf(theta);

    setSVPWM(alpha, beta);
}

/* ================= RAMP ================= */
static void updateRamp(float dt)
{
    float step = rampRpmPerSecond * dt;

    if (currentMechanicalRpm < targetMechanicalRpm)
    {
        currentMechanicalRpm += step;
        if (currentMechanicalRpm > targetMechanicalRpm)
            currentMechanicalRpm = targetMechanicalRpm;
    }
    else
    {
        currentMechanicalRpm -= step;
        if (currentMechanicalRpm < targetMechanicalRpm)
            currentMechanicalRpm = targetMechanicalRpm;
    }

    electricalRps = currentMechanicalRpm * pole_pairs / 60.0f;
}

/* ================= ALIGN ROTOR ================= */
static void alignRotor()
{
    for (int i = 0; i < 600; i++)
    {
        openLoopFOC(0.0f, startBoost);
        delay(2);
    }
}

/* ================= SETUP ================= */
void setup()
{
    delay(1000);

    setupPWM();

    alignRotor();

    electricalAngle = 0.0f;
}

/* ================= LOOP ================= */
void loop()
{
    static uint32_t last = micros();

    uint32_t now = micros();
    float dt = (now - last) * 1e-6f;
    last = now;

    if (dt <= 0.0f || dt > 0.05f)
        dt = 0.001f;

    updateRamp(dt);

    electricalAngle += 6.2831853f * electricalRps * dt;

    while (electricalAngle > 6.2831853f)
        electricalAngle -= 6.2831853f;

    float uq = (currentMechanicalRpm < 5.0f) ? startBoost : targetUqVolts;

    openLoopFOC(electricalAngle, uq);
}