#include "stm32f1xx_hal.h" // ⚠️ เปลี่ยนตามตระกูลบอร์ด เช่น stm32f4xx_hal.h หรือ stm32l4xx_hal.h

// ฟังก์ชันสำหรับตั้งค่าขา GPIO (แทน pinMode ใน Arduino)
void GPIO_Init(void) {
    // 1. เปิดสัญญาณนาฬิกา (Clock) ให้กับ Port C
    __HAL_RCC_GPIOC_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // 2. ตั้งค่าคุณสมบัติของขา PC13
    GPIO_InitStruct.Pin = GPIO_PIN_13;         // เลือกขา 13
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP; // ตั้งเป็น Output แบบ Push-Pull
    GPIO_InitStruct.Pull = GPIO_NOPULL;        // ไม่ใช้ Pull-up / Pull-down
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;// ความเร็วสัญญาณต่ำ (ประหยัดพลังงาน)
    
    // 3. สั่งให้ฟังก์ชัน HAL เริ่มทำงานตามที่เราตั้งค่าไว้ที่ Port C
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}

int main(void) {
    // รีเซ็ตระบบและเตรียมความพร้อมของ HAL Library
    HAL_Init();

    // เรียกฟังก์ชันตั้งค่าขาโครงสร้างที่เราเขียนไว้ด้านบน
    GPIO_Init();

    // Loop ทำงานหลัก (แทน void loop() ใน Arduino)
    while (1) {
        // สลับสถานะไฟ (Toggle) ที่ Port C ขา 13
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);

        // หน่วงเวลา 500 มิลลิวินาที (แทน delay() ใน Arduino)
        HAL_Delay(500); 
    }
}

// ฟังก์ชันนับเวลาของ HAL (จำเป็นต้องมีเพื่อให้ HAL_Delay ทำงานได้แม่นยำ)
void SysTick_Handler(void) {
    HAL_IncTick();
}