# BLDC SimpleFOC

![PlatformIO](https://img.shields.io/badge/PlatformIO-STM32-orange?style=for-the-badge)
![MCU](https://img.shields.io/badge/MCU-STM32F405RG-1f6feb?style=for-the-badge)
![Firmware](https://img.shields.io/badge/Firmware-BLDC%206--Step-2ea44f?style=for-the-badge)
![Status](https://img.shields.io/badge/Status-Prototype-yellow?style=for-the-badge)

Firmware prototype for driving a BLDC motor with a 6-step commutation sequence on an STM32F405RG target. The project is set up for PlatformIO and STM32Cube HAL.

> โปรเจกต์นี้เป็นตัวอย่าง/ต้นแบบสำหรับควบคุมมอเตอร์ BLDC ด้วย PWM 3 เฟสบน STM32 เหมาะสำหรับใช้ทดลองกับวงจร inverter/driver ที่มีการป้องกันกระแสและแรงดันเรียบร้อยแล้ว

## Highlights

- 6-step commutation loop for BLDC motor control
- TIM1 PWM and complementary PWM output usage
- PlatformIO project structure
- STM32Cube HAL based firmware
- Simple starting point for testing gate-driver and inverter hardware

## Hardware Target

| Item | Value |
| --- | --- |
| MCU board | `genericSTM32F405RG` |
| Framework | `stm32cube` |
| Main timer | `TIM1` |
| PWM channels | `CH1`, `CH2`, `CH3` with complementary outputs |
| Project tool | PlatformIO |

## Project Structure

```text
.
├── firmware/         # PlatformIO firmware project
│   ├── include/      # Shared header files
│   ├── lib/          # Private project libraries
│   ├── src/
│   │   └── main.c    # BLDC 6-step commutation firmware
│   ├── test/         # PlatformIO test folder
│   └── platformio.ini
├── hardware/         # Schematics, PCB, simulation, and BOM files
└── README.md
```

## Getting Started

1. Install [Visual Studio Code](https://code.visualstudio.com/).
2. Install the [PlatformIO IDE extension](https://platformio.org/install/ide?install=vscode).
3. Clone this repository.

```bash
git clone https://github.com/poommyroboticcamera-netizen/BLDC-SimpleFOC.git
cd BLDC-SimpleFOC/firmware
```

4. Build the firmware.

```bash
pio run
```

5. Upload to the board when the STM32 programmer/debugger is connected.

```bash
pio run --target upload
```

## Safety Notes

- Test the PWM outputs with an oscilloscope before connecting a motor.
- Use a current-limited power supply for the first hardware test.
- Confirm dead-time, gate-driver logic, and phase order before applying full voltage.
- Add emergency stop handling before using the firmware with high-power hardware.

## Roadmap

- [ ] Add pinout diagram and wiring table
- [ ] Add `main.h` / STM32Cube generated configuration files if they are required for standalone builds
- [ ] Add speed ramp instead of fixed-delay commutation
- [ ] Add current/voltage protection logic
- [ ] Add sensor or sensorless feedback
- [ ] Add build workflow badge after CI is configured

## Repository

GitHub: [poommyroboticcamera-netizen/BLDC-SimpleFOC](https://github.com/poommyroboticcamera-netizen/BLDC-SimpleFOC)
