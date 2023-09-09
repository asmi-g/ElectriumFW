#ifndef REGENBRAKING_H
#define REGENBRAKING_H

#include <SimpleFOC.h>
#include <Arduino.h>

// Constants for regenerative braking
const float MAX_REGEN_BRAKE_LEVEL = 100.0; // Maximum regenerative braking level (%)
const float MIN_REGEN_BRAKE_LEVEL = 0.0;   // Minimum regenerative braking level (%)

// Variables for regenerative braking
float regenBrakeLevel = 50.0;  // Default regenerative braking level (%)
bool regenBrakeEnabled = true; // Regenerative braking enabled by default

// Hall sensor instance
HallSensor sensor = HallSensor(2, 3, 4, 11);

// Interrupt routine initialization
void doA() { sensor.handleA(); }
void doB() { sensor.handleB(); }
void doC() { sensor.handleC(); }
PciListenerImp listenA(sensor.pinA, doA);
PciListenerImp listenB(sensor.pinB, doB);
PciListenerImp listenC(sensor.pinC, doC);

// DRV8833 motor controller pin assignments
const int motorIn1 = 9; // Motor input 1
const int motorIn2 = 5; // Motor input 2

// Battery voltage (adjust as per your battery specifications)
float batteryVoltage = 24.0; // Battery voltage in volts

// Set the voltage and limit for the motor driver
void setupMotorDriver()
{
    // Configure the motor controller pins
    pinMode(motorIn1, OUTPUT);
    pinMode(motorIn2, OUTPUT);
}

// Set up the hall sensors and motor driver
void setupSimpleFOC()
{
    // Set up the hall sensors
    sensor.pullup = Pullup::USE_EXTERN;
    sensor.init();
    PciManager.registerListener(&listenA);
    PciManager.registerListener(&listenB);
    PciManager.registerListener(&listenC);
}

// Functions for regenerative braking

// Set the regenerative braking level (0-100%)
void setRegenBrakeLevel(float level)
{
    // Clamp the regen brake level between the minimum and maximum values
    regenBrakeLevel = constrain(level, MIN_REGEN_BRAKE_LEVEL, MAX_REGEN_BRAKE_LEVEL);
}

// Get the current regenerative braking level
float getRegenBrakeLevel()
{
    return regenBrakeLevel;
}

// Enable or disable regenerative braking
void enableRegenBraking(bool enable)
{
    regenBrakeEnabled = enable;
}

// Apply regenerative braking based on the regen brake level and enable/disable status
void applyRegenBraking()
{
    // Check if regenerative braking is enabled
    if (regenBrakeEnabled)
    {
        // Calculate the regenerative braking voltage
        float regenVoltage = driver.voltage_power_supply * regenBrakeLevel / 100.0;

        // Calculate the power to divert back to the battery
        float powerDiverted = regenVoltage * driver.voltage_q;

        // Divert the power back to the battery
        float current = powerDiverted / batteryVoltage;

        // Apply regenerative braking to the motor controller
        if (current >= 0)
        {
            // Set motor inputs for forward braking
            analogWrite(motorIn1, 0);
            analogWrite(motorIn2, current * 255);
        }
        else
        {
            // Set motor inputs for reverse braking
            analogWrite(motorIn1, -current * 255);
            analogWrite(motorIn2, 0);
        }
    }
}

#endif
