<div align="center">

# BLDC SimpleFOC

Custom STM32F405RG firmware and hardware reference files for a 3-phase BLDC inverter.

![PlatformIO](https://img.shields.io/badge/PlatformIO-STM32-orange?style=for-the-badge)
![Arduino](https://img.shields.io/badge/Framework-Arduino-00979d?style=for-the-badge)
![FreeRTOS](https://img.shields.io/badge/RTOS-FreeRTOS-2f6f44?style=for-the-badge)
![MCU](https://img.shields.io/badge/MCU-STM32F405RG-1f6feb?style=for-the-badge)
![Status](https://img.shields.io/badge/Status-Hardware%20Bring--Up-f4c430?style=for-the-badge)

</div>

---

## Overview

`BLDC-SimpleFOC` is a hardware bring-up project for a custom 3-phase BLDC motor driver based on an STM32F405RG. The current firmware drives TIM1 complementary PWM outputs directly, adds dead-time at the timer level, reads phase voltage feedback with ADC pins, and exposes simple serial commands for early bench testing.

This repository is intended for controlled inverter testing, not finished production motor control. Start with a current-limited bench supply, low duty cycle, and oscilloscope verification before connecting a real motor load.

## What Is Included

| Area | Details |
| --- | --- |
| Firmware target | STM32F405RG using PlatformIO and Arduino |
| Runtime model | STM32duino FreeRTOS tasks |
| PWM output | TIM1 center-aligned 6-PWM complementary outputs |
| Switching frequency | `20 kHz` |
<<<<<<< HEAD
| Dead-time | `1.5us` configured in TIM1 BDTR |
=======
| Dead-time | `1.5 us` configured in TIM1 BDTR |
>>>>>>> 4c121442047dd9c6b4e7016d9e61324d03763788
| Control mode | Open-loop sinusoidal V/f style test waveform |
| Monitoring | Serial output plus phase voltage ADC reads |
| Hardware files | Schematic PDF, 3D STEP model, and firmware flow diagram |

## Repository Layout

```text
BLDC-SimpleFOC/
|-- firmware/
|   |-- include/        Project headers, when shared declarations are added
|   |-- lib/            Private PlatformIO libraries, when the firmware grows
|   |-- src/
|   |   `-- main.cpp    Main STM32 PWM, FreeRTOS, ADC, and serial logic
|   |-- test/           PlatformIO unit/integration test area
|   `-- platformio.ini  Board, framework, upload, monitor, and library config
|-- hardware/
|   |-- 3D_PCB_2026-06-15.step
|   |-- SCH_BLDC Drive_2026-06-15.pdf
|   |-- Custom FOC Firmware.drawio
|   |-- Custom FOC Firmware.drawio.png
|   |-- Custom FOC Firmware.drawio.svg
|   `-- README.md
|-- .gitattributes
|-- .gitignore
`-- README.md
```

## Hardware Pin Map

### PWM Outputs

TIM1 is configured for complementary high-side and low-side PWM outputs.

| Motor phase | High-side PWM | Low-side PWM | Timer channel |
| --- | --- | --- | --- |
| U | `PA8` | `PB13` | `TIM1_CH1 / TIM1_CH1N` |
| V | `PA9` | `PB14` | `TIM1_CH2 / TIM1_CH2N` |
| W | `PA10` | `PB15` | `TIM1_CH3 / TIM1_CH3N` |

### Phase Voltage Feedback

| Signal | MCU pin | Notes |
| --- | --- | --- |
| Phase A voltage | `PA0` | 12-bit ADC read |
| Phase B voltage | `PA1` | 12-bit ADC read |
| Phase C voltage | `PA2` | 12-bit ADC read |

The firmware currently assumes a phase voltage divider of:

```cpp
R1_PHASE = 56000.0f
R2_PHASE = 2200.0f
```

Adjust these constants in `firmware/src/main.cpp` if the resistor values on the board change.

## Firmware Behavior

The firmware creates two FreeRTOS tasks:

| Task | Period | Purpose |
| --- | ---: | --- |
| `TaskMotorControl` | `1 ms` | Updates the sinusoidal 3-phase PWM duty registers |
| `TaskMonitor` | `200 ms` | Reads serial commands, samples phase voltage ADCs, and prints telemetry |

Default runtime values:

| Setting | Default |
| --- | ---: |
| PWM frequency | `20 kHz` |
| Dead-time | `1.5us` |
| Initial duty | `5%` |
| Initial rotation speed | `1.0 deg/loop` |
| Serial baud | `115200` |

## Quick Start

Install VS Code with the PlatformIO extension, then clone and open the firmware project.

```bash
git clone https://github.com/poommyroboticcamera-netizen/BLDC-SimpleFOC.git
cd BLDC-SimpleFOC/firmware
```

Build the firmware:

```bash
pio run
```

Upload with ST-Link:

```bash
pio run --target upload
```

Open the serial monitor:

```bash
pio device monitor -b 115200
```

## Serial Commands

Commands are read from the serial monitor as one letter plus a numeric value.

| Command | Range | Example | Meaning |
| --- | --- | --- | --- |
| `D<value>` | `0` to `100` | `D5` | Set PWM duty percentage |
| `S<value>` | `0` to `20` | `S2.5` | Set electrical rotation speed in degrees per loop |

Recommended first commands:

```text
D0
S1
D2
D5
```

Increase duty slowly while watching the bench supply current and oscilloscope waveforms.

## Bring-Up Checklist

1. Confirm the schematic matches the PWM and ADC pin map above.
2. Power only the logic side and verify the serial boot message.
3. Scope `PA8`, `PA9`, `PA10`, `PB13`, `PB14`, and `PB15` before enabling motor power.
4. Confirm complementary PWM timing and dead-time.
5. Enable the gate driver with a current-limited supply.
6. Start with `D0`, then raise duty in small steps.
7. Verify phase order before connecting a mechanical load.
8. Add current sensing, fault handling, and closed-loop feedback before high-power testing.

## Hardware Documents

The hardware folder contains the current reference files:

| File | Purpose |
| --- | --- |
| [`SCH_BLDC Drive_2026-06-15.pdf`](hardware/SCH_BLDC%20Drive_2026-06-15.pdf) | Schematic export |
| [`3D_PCB_2026-06-15.step`](hardware/3D_PCB_2026-06-15.step) | 3D PCB model for mechanical review |
| [`Custom FOC Firmware.drawio.png`](hardware/Custom%20FOC%20Firmware.drawio.png) | Firmware flow diagram preview |
| [`Custom FOC Firmware.drawio`](hardware/Custom%20FOC%20Firmware.drawio) | Editable draw.io diagram |

![Firmware flow diagram](hardware/Custom%20FOC%20Firmware.drawio.png)

## Roadmap

- [ ] Clean up firmware comments and source encoding
- [ ] Add gate-driver enable and fault input handling
- [ ] Add current sensing and over-current protection
- [ ] Add magnetic encoder or Hall sensor feedback
- [ ] Add closed-loop FOC control after hardware validation
- [ ] Add known-good motor settings and oscilloscope captures
- [ ] Add PlatformIO CI build check

## Safety Notes

- Treat the inverter as high-energy hardware even at low voltage.
- Use a current-limited bench supply during first tests.
- Keep duty cycle low until all six PWM channels are verified.
- Do not run high power without fault handling and current protection.
- Disconnect motor power before changing wiring or probing low-side gate signals.

## Links

- Repository: [poommyroboticcamera-netizen/BLDC-SimpleFOC](https://github.com/poommyroboticcamera-netizen/BLDC-SimpleFOC)
- PlatformIO documentation: [docs.platformio.org](https://docs.platformio.org/)
- STM32duino documentation: [github.com/stm32duino](https://github.com/stm32duino)
