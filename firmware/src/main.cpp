#include <Arduino.h>
#include <SimpleFOC.h>

// STM32 TIM1 hardware 6-PWM pins. Change these if your board wiring is different.
#ifndef PIN_PHASE_UH
#define PIN_PHASE_UH PA8
#endif

#ifndef PIN_PHASE_UL
#define PIN_PHASE_UL PB13
#endif

#ifndef PIN_PHASE_VH
#define PIN_PHASE_VH PA9
#endif

#ifndef PIN_PHASE_VL
#define PIN_PHASE_VL PB14
#endif

#ifndef PIN_PHASE_WH
#define PIN_PHASE_WH PA10
#endif

#ifndef PIN_PHASE_WL
#define PIN_PHASE_WL PB15
#endif

// Set this to the gate-driver enable pin if your power stage needs one.
#ifndef PIN_DRIVER_ENABLE
#define PIN_DRIVER_ENABLE NOT_SET
#endif

#ifndef MOTOR_POLE_PAIRS
#define MOTOR_POLE_PAIRS 7
#endif

#ifndef SUPPLY_VOLTAGE
#define SUPPLY_VOLTAGE 12.0f
#endif

#ifndef VOLTAGE_LIMIT
#define VOLTAGE_LIMIT 2.0f
#endif

#ifndef VELOCITY_LIMIT
#define VELOCITY_LIMIT 20.0f
#endif

#ifndef START_TARGET_VELOCITY
#define START_TARGET_VELOCITY 2.0f
#endif

BLDCMotor motor = BLDCMotor(MOTOR_POLE_PAIRS);
BLDCDriver6PWM driver = BLDCDriver6PWM(
    PIN_PHASE_UH, PIN_PHASE_UL,
    PIN_PHASE_VH, PIN_PHASE_VL,
    PIN_PHASE_WH, PIN_PHASE_WL,
    PIN_DRIVER_ENABLE, PIN_DRIVER_ENABLE, PIN_DRIVER_ENABLE);

Commander commander = Commander(Serial);
float target_velocity = START_TARGET_VELOCITY;

void onTargetVelocity(char *cmd)
{
    commander.scalar(&target_velocity, cmd);
}

void printConfig()
{
    Serial.println();
    Serial.println(F("BLDC SimpleFOC open-loop velocity controller"));
    Serial.print(F("Pole pairs: "));
    Serial.println(MOTOR_POLE_PAIRS);
    Serial.print(F("Supply voltage: "));
    Serial.println(SUPPLY_VOLTAGE);
    Serial.print(F("Voltage limit: "));
    Serial.println(VOLTAGE_LIMIT);
    Serial.print(F("Velocity limit: "));
    Serial.println(VELOCITY_LIMIT);
    Serial.println(F("Command: T<rad/s>, for example T3.5 or T0"));
    Serial.println();
}

void setup()
{
    Serial.begin(115200);
    delay(1000);

    SimpleFOCDebug::enable(&Serial);
    printConfig();

    driver.voltage_power_supply = SUPPLY_VOLTAGE;
    driver.voltage_limit = VOLTAGE_LIMIT;
    driver.init();

    motor.linkDriver(&driver);
    motor.controller = MotionControlType::velocity_openloop;
    motor.voltage_limit = VOLTAGE_LIMIT;
    motor.velocity_limit = VELOCITY_LIMIT;
    motor.useMonitoring(Serial);
    motor.init();

    commander.add('T', onTargetVelocity, "target velocity [rad/s]");

    Serial.println(F("Motor ready."));
}

void loop()
{
    motor.move(target_velocity);
    commander.run();
}
