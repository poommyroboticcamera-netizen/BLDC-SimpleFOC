# Hardware

Hardware reference files for the custom STM32F405RG 3-phase BLDC driver.

## Files

| File | Description |
| --- | --- |
| [`SCH_BLDC Drive_2026-06-15.pdf`](SCH_BLDC%20Drive_2026-06-15.pdf) | Schematic export for electrical review |
| [`3D_PCB_2026-06-15.step`](3D_PCB_2026-06-15.step) | STEP model for mechanical fit checks and enclosure planning |
| [`Custom FOC Firmware.drawio`](Custom%20FOC%20Firmware.drawio) | Editable draw.io firmware/control-flow diagram |
| [`Custom FOC Firmware.drawio.png`](Custom%20FOC%20Firmware.drawio.png) | PNG preview of the firmware/control-flow diagram |
| [`Custom FOC Firmware.drawio.svg`](Custom%20FOC%20Firmware.drawio.svg) | SVG version of the diagram for scalable viewing |

## Firmware Flow Preview

![Firmware flow diagram](Custom%20FOC%20Firmware.drawio.png)

## Board Interface Summary

| Function | Pins |
| --- | --- |
| High-side PWM | `PA8`, `PA9`, `PA10` |
| Low-side PWM | `PB13`, `PB14`, `PB15` |
| Phase voltage ADC | `PA0`, `PA1`, `PA2` |

## Review Checklist

- Confirm PWM pins match the gate-driver inputs in the schematic.
- Confirm the ADC phase-voltage divider values match the firmware constants.
- Verify gate-driver enable, fault, and protection pins before high-power tests.
- Check copper clearance, creepage, and thermal paths for the target bus voltage.
- Use the STEP file to check connector access, heatsink clearance, and mounting holes.

## Suggested Future Structure

As the hardware package grows, split files into focused folders:

```text
hardware/
|-- schematics/
|-- pcb/
|-- mechanical/
|-- simulation/
|-- bom/
`-- datasheets/
```
