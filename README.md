<div align="center">

# BLDC SimpleFOC

STM32F405RG firmware template for a 3-phase BLDC inverter using PlatformIO, Arduino, and SimpleFOC.

![PlatformIO](https://img.shields.io/badge/PlatformIO-STM32-orange?style=for-the-badge)
![SimpleFOC](https://img.shields.io/badge/SimpleFOC-2.4-19a974?style=for-the-badge)
![MCU](https://img.shields.io/badge/MCU-STM32F405RG-1f6feb?style=for-the-badge)
![Status](https://img.shields.io/badge/Status-Prototype-f4c430?style=for-the-badge)

</div>

---

## Overview

`BLDC-SimpleFOC` is a clean starting template for bringing up a 6-PWM BLDC motor driver on STM32. It replaces the early STM32Cube skeleton with a buildable SimpleFOC firmware layout, conservative defaults, and serial tuning commands for bench testing.

> This project is for controlled hardware bring-up. Start with a current-limited power supply, low voltage limits, and oscilloscope checks before connecting a motor.

## Feature Snapshot

| Area | Included |
| --- | --- |
| Motor control | SimpleFOC open-loop velocity control |
| PWM topology | 6-PWM high/low gate-driver outputs |
| Target board | `genericSTM32F405RG` |
| Firmware stack | PlatformIO + Arduino + SimpleFOC |
| Runtime tuning | Serial Commander target velocity command |
| Safety defaults | Low voltage limit and explicit hardware checklist |

## Repository Layout

```text
BLDC-SimpleFOC/
|-- firmware/
|   |-- include/
|   |-- lib/
|   |-- src/
|   |   `-- main.cpp
|   |-- test/
|   `-- platformio.ini
|-- hardware/
|   `-- README.md
|-- .vscode/
|-- .gitignore
`-- README.md
```

## Firmware Defaults

| Setting | Default | Where to edit |
| --- | ---: | --- |
| Pole pairs | `7` | `firmware/platformio.ini` |
| Supply voltage | `12.0 V` | `firmware/platformio.ini` |
| Voltage limit | `2.0 V` | `firmware/platformio.ini` |
| Velocity limit | `20.0 rad/s` | `firmware/platformio.ini` |
| Start velocity | `2.0 rad/s` | `firmware/src/main.cpp` |
| Serial baud | `115200` | `firmware/platformio.ini` |

## Default Pin Map

STM32 TIM1-compatible 6-PWM wiring:

| Phase | High-side PWM | Low-side PWM |
| --- | --- | --- |
| U | `PA8` | `PB13` |
| V | `PA9` | `PB14` |
| W | `PA10` | `PB15` |

If your gate driver has an enable pin, define `PIN_DRIVER_ENABLE` in `firmware/platformio.ini` or update `firmware/src/main.cpp`.

## Quick Start

Install PlatformIO, then build from the firmware folder:

```bash
cd firmware
pio run
```

Upload to the STM32 board:

```bash
pio run --target upload
```

Open the serial monitor:

```bash
pio device monitor -b 115200
```

Set target velocity with SimpleFOC Commander:

```text
T0
T2.5
T-2.5
```

## Bring-Up Flow

1. Confirm MCU pin mapping against your schematic.
2. Power only the logic side and check firmware boot logs on serial.
3. Scope all six PWM outputs without motor power.
4. Enable the gate driver with a current-limited bench supply.
5. Start with `T0`, then small commands such as `T1` or `T2`.
6. Increase `VOLTAGE_LIMIT` only after phase order and switching behavior are verified.

## Safety Checklist

- Use a current-limited power supply for first power-up.
- Keep `VOLTAGE_LIMIT` low until the waveform and phase order are proven.
- Confirm dead-time and gate-driver shoot-through protection.
- Verify gate-driver enable polarity before enabling motor power.
- Add fault input, current sensing, over-voltage, and emergency-stop logic before high-power testing.
- Move to closed-loop FOC with a sensor before running under real load.

## Roadmap

- [ ] Add board-specific schematic and pinout images
- [ ] Add magnetic encoder or Hall sensor closed-loop mode
- [ ] Add current-sense configuration
- [ ] Add gate-driver fault and enable handling
- [ ] Add automated PlatformIO CI build
- [ ] Add hardware test notes and known-good motor settings

## Links

- Repository: [poommyroboticcamera-netizen/BLDC-SimpleFOC](https://github.com/poommyroboticcamera-netizen/BLDC-SimpleFOC)
- SimpleFOC documentation: [docs.simplefoc.com](https://docs.simplefoc.com/)
- PlatformIO documentation: [docs.platformio.org](https://docs.platformio.org/)
