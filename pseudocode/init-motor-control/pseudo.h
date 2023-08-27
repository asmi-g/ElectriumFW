#include <SimpleFOC.h>
// PART A: Set up the hall sensors
// Hall sensor instance
// HallSensor(int hallA, int hallB , int hallC , int pp)
//  - hallA, hallB, hallC    - HallSensor A, B and C pins
//  - pp                     - pole pairs
HallSensor sensor = HallSensor(2, 3, 4, 11);

// Interrupt routine initialization
// channel A and B callbacks
void doA() { sensor.handleA(); }
void doB() { sensor.handleB(); }
void doC() { sensor.handleC(); }

// sensor interrupt init
PciListenerImp listenA(sensor.pinA, doA);
PciListenerImp listenB(sensor.pinB, doB);
PciListenerImp listenC(sensor.pinC, doC);
// END OF PART A

// PART B: Set up motor driver
// BLDCDriver3PWM( pin_pwmA, pin_pwmB, pin_pwmC, enable (optional))
BLDCDriver3PWM driver = BLDCDriver3PWM(9, 5, 6, 8);
// END OF PART B

// PART C: Instantiate motor
// BLDCMotor( pole_pairs , ( phase_resistance, KV_rating  optional) )
BLDCMotor motor = BLDCMotor(11, 9.75);
// END OF PART C

// PART D: Instantiate commander for user communication
// Note: the input of our ebike is a throttle device, not serial input
Commander command = Commander(Serial);
void setup()
{
  // START OF INIT position sensors
  // monitoring port
  Serial.begin(115200);
  // check if you need internal pullups
  sensor.pullup = Pullup::USE_EXTERN;
  // initialize sensor hardware
  sensor.init();
  // interrupt initialization
  PciManager.registerListener(&listenA);
  PciManager.registerListener(&listenB);
  PciManager.registerListener(&listenC);
  Serial.println("Sensor ready");
  _delay(1000);
  // END of INIT position sensors

  // START OF INIT motor driver
  // pwm frequency to be used [Hz]
  // for stm32, 25k Hz is the default frequency. SimpleFOC recommends staying at 20k Hz
  driver.pwm_frequency = 25000;
  // DC power supply voltage [V]. This value will depend on our battery, 12V is used below
  driver.voltage_power_supply = 12;
  // Max DC voltage allowed - default voltage_power_supply
  driver.voltage_limit = 12;
  // Configures the driver with all the necessary hardware and software configurations
  driver.init();
  // END OF INIT motor driver

  // START OF motor setup
  // link the motor to the sensor
  motor.linkSensor(&sensor);
  // link the motor to the driver
  motor.linkDriver(&driver);
  // define motor voltage and velocity limits
  motor.voltage_limit = 3;  // [V] - if phase resistance not defined
  motor.velocity_limit = 5; // [rad/s] cca 50rpm
  // set open loop control config
  motor.controller = MotionControlType::velocity_openloop;
  // initialize motor
  motor.init();

  // add target command T
  // Note: this will change since our input form is the throttle
  command.add('T', doTarget, "target velocity");

  Serial.begin(115200);
  Serial.println("Motor ready!");
  Serial.println("Set target velocity [rad/s]");
  _delay(1000);
}

void loop()
{
  // START OF SENSOR POSITIONING TESTING
  // IMPORTANT - call as frequently as possible
  // update the sensor values
  sensor.update();
  // display the angle and the angular velocity to the terminal
  Serial.print(sensor.getAngle());
  Serial.print("\t");
  Serial.println(sensor.getVelocity());
  // END OF SENSOR POSITIONING

  // START OF TESTING THE MOTOR DRIVER. More code to configure the positions/phases of the driver: https://docs.simplefoc.com/bldcdriver3pwm
  Serial.print("Driver init ");
  // init driver
  if (driver.init())
    Serial.println("success!");
  else
  {
    Serial.println("failed!");
    return;
  }
  // END OF TESTING MOTOR DRIVER

  // START OF MOTOR CONTROL open loop velocity movement
  // using motor.voltage_limit and motor.velocity_limit
  motor.move();

  // user communication (input to control velocity)
  command.run();
}