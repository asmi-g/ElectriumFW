#include <SimpleFOC.h>
#include <Arduino.h>
#include "idle.h"

/*
    IDLE MODE TESTING CONSTRAINTS:
        - Conditions to check:
            - Not reading any input from the throttle (analog value)
            - Not reading any value from the speedometer (float value)
        - Verification while testing:
            - While the conditions above are true, the motor should stop running
*/



//basic setup, will be integrated into general forward drive subsystem
void setup(){
    //set voltage to half
    driver.voltage_power_supply = 6;
    //set max voltage limit to half
    driver.voltage_limit = 6;
    driver.init();
}

void loop(){
    //get speed from speedometer, need to configure, placeholder for now
    float speed = sensor.getVelocity(); 

    //get input from throttle, need to configure, placeholder for now
    bool throttle_input = motor.isMotorEnabled();

    //while no throttle input and bike is stationary
    while (speed == 0 && !throttle_input){
        //initiate idle mode
        idle_mode();
    }

    //continue with main bike functionality when loop is broken
}

//example loop funcion
void checkECO() {
    //using interrupts, if switch flipped, change velocity to 16kmh
    if (ecoModeOn) {
    Serial.println("Eco mode is ON");
    // Perform actions specific to eco mode
  } else {
    Serial.println("Eco mode is OFF");
    // Perform actions for regular mode
  }
}

void idle_mode(){
    //writes 0 to enable pin for the driver
    driver.enable_active_high = false;
}
