# Firmware

PlatformIO firmware for the custom STM32F405RG 3-phase BLDC inverter.

## Stack

| Item | Value |
| --- | --- |
| Board | `genericSTM32F405RG` |
| Platform | `ststm32` |
| Framework | Arduino |
| RTOS | STM32duino FreeRTOS |
| Upload protocol | ST-Link |
| Serial monitor | `115200` baud |

## Source Layout

```text
firmware/
|-- include/        Shared headers, when needed
|-- lib/            Private PlatformIO libraries, when needed
|-- src/
|   `-- main.cpp    Main PWM, ADC, serial, and FreeRTOS logic
|-- test/           PlatformIO tests
|-- platformio.ini
`-- README.md
```

## Current Control Logic

The current firmware is an open-loop hardware bring-up program:

- Configures TIM1 for center-aligned complementary 6-PWM output.
- Applies `360 ns` dead-time in the advanced timer.
- Generates a 3-phase sinusoidal duty pattern.
- Reads phase voltage feedback on `PA0`, `PA1`, and `PA2`.
- Uses FreeRTOS to separate motor PWM updates from serial monitoring.

## Build

```bash
pio run
```

## Upload

```bash
pio run --target upload
```

## Monitor

```bash
pio device monitor -b 115200
```

## Serial Commands

| Command | Example | Description |
| --- | --- | --- |
| `D<value>` | `D5` | Set duty cycle in percent, from `0` to `100` |
| `S<value>` | `S2.5` | Set electrical speed in degrees per control loop, from `0` to `20` |

For first power-up, begin with:

```text
D0
S1
D2
```

## Hardware Pins

| Function | Pins |
| --- | --- |
| High-side PWM | `PA8`, `PA9`, `PA10` |
| Low-side PWM | `PB13`, `PB14`, `PB15` |
| Phase voltage ADC | `PA0`, `PA1`, `PA2` |

## Bring-Up Notes

- Verify all six PWM outputs with an oscilloscope before enabling motor power.
- Keep the duty cycle low during first tests.
- Use a current-limited bench supply.
- Add current sensing and fault handling before high-power operation.
