<div align="center">

# BLDC SimpleFOC

### Open-loop BLDC bring-up firmware for a custom STM32F405RG inverter

[![PlatformIO](https://img.shields.io/badge/PlatformIO-STM32-orange?style=for-the-badge&logo=platformio)](https://platformio.org/)
[![Framework](https://img.shields.io/badge/Framework-Arduino-00979D?style=for-the-badge&logo=arduino)](https://www.arduino.cc/)
[![MCU](https://img.shields.io/badge/MCU-STM32F405RG-03234B?style=for-the-badge)](https://www.st.com/en/microcontrollers-microprocessors/stm32f405rg.html)
[![Status](https://img.shields.io/badge/Status-Hardware%20Bring--Up-F4C430?style=for-the-badge)](#safety-and-bring-up)

**Firmware | Hardware references | Safe bench validation**

[Getting started](#getting-started) | [Firmware](#firmware-at-a-glance) | [Pin map](#pin-map) | [Hardware](#hardware-references) | [Safety](#safety-and-bring-up)

</div>

---

> [!WARNING]
> **Experimental inverter firmware.** This project is for controlled bench testing only. It does not yet implement current limiting, fault handling, or closed-loop control. Use a current-limited supply and verify all gate-drive signals with an oscilloscope before connecting a motor.

## Overview

BLDC SimpleFOC is the firmware and hardware reference package for a custom three-phase BLDC inverter built around an STM32F405RG. The firmware configures TIM1 directly for complementary six-PWM output and generates an open-loop voltage vector with space-vector PWM (SVPWM).

| Project | Current implementation |
| --- | --- |
| **Target** | Custom three-phase BLDC inverter |
| **Controller** | STM32F405RG |
| **Toolchain** | PlatformIO and Arduino framework |
| **PWM** | TIM1 center-aligned complementary 6-PWM at 20 kHz |
| **Control mode** | Open-loop field-oriented voltage vector with SVPWM |
| **Project stage** | Hardware bring-up |

## Firmware at a glance

The startup sequence aligns the rotor, ramps toward the configured mechanical speed, and continuously updates the SVPWM duty cycles. All values below are source defaults from [`firmware/src/main.cpp`](firmware/src/main.cpp); they are not guaranteed motor-safe limits.

| Parameter | Default | Notes |
| --- | ---: | --- |
| Bus voltage constant | 24 V | Used by the SVPWM duty calculation |
| PWM frequency | 20 kHz | TIM1 center-aligned mode |
| Timer clock | 168 MHz | Used to derive TIM1 timing |
| Dead-time | 1.0 us | Configured through TIM1 BDTR |
| Pole pairs | 6 | Used to calculate electrical speed |
| Target mechanical speed | 440 RPM | Open-loop target |
| Ramp rate | 120 RPM/s | Speed transition limit |
| Target `Uq` voltage | 4.2 V | Applied after rotor alignment |

## Pin map

TIM1 drives complementary high-side and low-side PWM pairs for the three motor phases.

| Phase | High-side PWM | Low-side PWM | Timer channel |
| --- | --- | --- | --- |
| U | `PA8` | `PB13` | `TIM1_CH1 / TIM1_CH1N` |
| V | `PA9` | `PB14` | `TIM1_CH2 / TIM1_CH2N` |
| W | `PA10` | `PB15` | `TIM1_CH3 / TIM1_CH3N` |

Confirm every signal against the schematic and gate-driver inputs before applying motor power.

## Getting started

### Prerequisites

- [VS Code](https://code.visualstudio.com/)
- [PlatformIO extension](https://platformio.org/install/ide?install=vscode)
- ST-Link connected to the target board

### Build and upload

```bash
git clone https://github.com/poommyroboticcamera-netizen/BLDC-SimpleFOC.git
cd BLDC-SimpleFOC/firmware

# Build
pio run

# Flash through ST-Link
pio run --target upload
```

## Safety and bring-up

Use this sequence before connecting a motor:

1. Compare the schematic with the pin map above.
2. Power only the logic side and verify normal startup.
3. Probe `PA8`, `PA9`, `PA10`, `PB13`, `PB14`, and `PB15` with an oscilloscope.
4. Verify complementary switching and dead-time at the gate-driver outputs.
5. Apply motor power through a current-limited bench supply.
6. Test at low voltage and stop immediately if current draw or temperature is unexpected.

Do not use this firmware for high-power operation until gate-enable control, fault inputs, current sensing, and validated protection behavior are in place.

## Hardware references

The `hardware/` directory contains the files needed for electrical, mechanical, and firmware-flow review.

| File | Use |
| --- | --- |
| [`SCH_BLDC Drive_2026-06-15.pdf`](hardware/SCH_BLDC%20Drive_2026-06-15.pdf) | Electrical schematic |
| [`3D_PCB_2026-06-15.step`](hardware/3D_PCB_2026-06-15.step) | Mechanical board model |
| [`Custom FOC Firmware.drawio`](hardware/Custom%20FOC%20Firmware.drawio) | Editable firmware-flow diagram |
| [`Custom FOC Firmware.drawio.png`](hardware/Custom%20FOC%20Firmware.drawio.png) | Diagram preview |

<p align="center">
  <img src="hardware/Custom%20FOC%20Firmware.drawio.png" alt="Firmware flow diagram" width="760" />
</p>

For a detailed hardware file guide, see [hardware/README.md](hardware/README.md).

## Repository layout

```text
BLDC-SimpleFOC/
|-- firmware/
|   |-- src/main.cpp        Open-loop control and direct TIM1 configuration
|   `-- platformio.ini      STM32F405RG PlatformIO environment
`-- hardware/               Schematic, STEP model, and flow diagrams
```

## Roadmap

- [ ] Add gate-driver enable and fault input handling
- [ ] Add current sensing and over-current protection
- [ ] Add position feedback (encoder or Hall sensors)
- [ ] Add closed-loop FOC control
- [ ] Publish known-good motor parameters and oscilloscope captures
- [ ] Add a PlatformIO CI build

## License

No license is currently specified for this repository. Contact the project owner before reusing the firmware or hardware.
