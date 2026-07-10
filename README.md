<div align="center">

# BLDC SimpleFOC

**STM32F405RG firmware and hardware references for a custom three-phase BLDC inverter.**

[![PlatformIO](https://img.shields.io/badge/PlatformIO-STM32-orange?style=flat-square)](https://platformio.org/)
[![Framework](https://img.shields.io/badge/Framework-Arduino-00979D?style=flat-square)](https://www.arduino.cc/)
[![MCU](https://img.shields.io/badge/MCU-STM32F405RG-03234B?style=flat-square)](https://www.st.com/en/microcontrollers-microprocessors/stm32f405rg.html)
[![Status](https://img.shields.io/badge/Status-hardware%20bring--up-F4C430?style=flat-square)](#safety--bring-up)

</div>

> [!WARNING]
> This is experimental inverter firmware for controlled bench testing. It does **not** yet implement current limiting, fault handling, or closed-loop control. Use a current-limited supply and verify all gate-drive signals with an oscilloscope before connecting a motor.

## At a glance

| | |
| --- | --- |
| Target | Custom three-phase BLDC inverter |
| Controller | STM32F405RG |
| Toolchain | PlatformIO + Arduino framework |
| PWM | TIM1, center-aligned complementary 6-PWM at 20 kHz |
| Control | Open-loop field-oriented voltage vector with SVPWM |
| Default bus voltage | 24 V |
| Project stage | Hardware bring-up |

## What this repository contains

```text
BLDC-SimpleFOC/
├── firmware/
│   ├── src/main.cpp        Open-loop control and direct TIM1 configuration
│   └── platformio.ini      STM32F405RG PlatformIO environment
└── hardware/
    ├── SCH_BLDC Drive_2026-06-15.pdf
    ├── 3D_PCB_2026-06-15.step
    └── Custom FOC Firmware.drawio[.png|.svg]
```

The `hardware/` directory includes the schematic, a mechanical STEP model, and an editable firmware-flow diagram. See its [hardware README](hardware/README.md) for a file-by-file guide.

## Firmware behavior

The firmware configures TIM1 directly and drives complementary PWM pairs for the three motor phases. At startup it briefly aligns the rotor, then ramps toward the configured mechanical speed and creates an open-loop voltage vector using space-vector PWM (SVPWM).

| Setting | Current value |
| --- | ---: |
| PWM frequency | 20 kHz |
| Timer clock | 168 MHz |
| Dead-time | 1.0 µs |
| Bus voltage constant | 24 V |
| Pole pairs | 6 |
| Target mechanical speed | 440 RPM |
| Ramp rate | 120 RPM/s |
| Target `Uq` voltage | 4.2 V |

These are source-level defaults, not guaranteed motor-safe operating limits. Review the constants at the top of [`firmware/src/main.cpp`](firmware/src/main.cpp) for your hardware and motor.

## Pin map

TIM1 supplies the complementary high-side and low-side outputs.

| Phase | High-side | Low-side | TIM1 channel |
| --- | --- | --- | --- |
| U | `PA8` | `PB13` | `CH1 / CH1N` |
| V | `PA9` | `PB14` | `CH2 / CH2N` |
| W | `PA10` | `PB15` | `CH3 / CH3N` |

Confirm these signals against the schematic and gate-driver connections before applying motor power.

## Quick start

### 1. Install the tooling

Install [VS Code](https://code.visualstudio.com/) and the [PlatformIO extension](https://platformio.org/install/ide?install=vscode), then clone the project:

```bash
git clone https://github.com/poommyroboticcamera-netizen/BLDC-SimpleFOC.git
cd BLDC-SimpleFOC/firmware
```

### 2. Build

```bash
pio run
```

### 3. Upload with ST-Link

```bash
pio run --target upload
```

## Safety & bring-up

Before connecting a motor:

1. Compare the schematic with the PWM pin map above.
2. Power only the logic side and confirm the board starts normally.
3. Scope `PA8`, `PA9`, `PA10`, `PB13`, `PB14`, and `PB15`.
4. Confirm complementary switching and dead-time at the gate-driver outputs.
5. Apply motor power from a current-limited bench supply.
6. Test at low voltage first; stop immediately if current or heating is unexpected.

Do not use this firmware for high-power operation until the hardware has gate-enable control, fault input handling, current sensing, and validated protection behavior.

## Hardware references

| File | Purpose |
| --- | --- |
| [`SCH_BLDC Drive_2026-06-15.pdf`](hardware/SCH_BLDC%20Drive_2026-06-15.pdf) | Schematic for electrical review |
| [`3D_PCB_2026-06-15.step`](hardware/3D_PCB_2026-06-15.step) | Board model for mechanical checks |
| [`Custom FOC Firmware.drawio`](hardware/Custom%20FOC%20Firmware.drawio) | Editable firmware-flow diagram |
| [`Custom FOC Firmware.drawio.png`](hardware/Custom%20FOC%20Firmware.drawio.png) | Diagram preview |

![Firmware flow diagram](hardware/Custom%20FOC%20Firmware.drawio.png)

## Roadmap

- [ ] Add gate-driver enable and fault input handling
- [ ] Add current sensing and over-current protection
- [ ] Add position feedback (encoder or Hall sensors)
- [ ] Add closed-loop FOC control
- [ ] Add known-good motor parameters and oscilloscope captures
- [ ] Add a PlatformIO CI build

## License

No license is currently specified for this repository. Contact the project owner before reusing the hardware or firmware.
