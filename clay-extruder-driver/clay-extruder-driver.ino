
/*
Clay extruder control

 modified Stepper Driver Control

 This program drives a stepper through a driver.
 The driver is attached to digital pins 8 and 9 and 3.3V of the Arduino.

 Stepper is controlled via BLE
 
 Created 28 May. 2020
 Modified 12 Aug. 2023
 by Kaiko Kivi

 */
#include <Arduino.h>
#include <ArduinoBLE.h>
#include "src/StepperDriver/StepperDriver.h"

// BLE attributes for EXTURDER controller
const char *BLE_LOCAL_NAME = "Extruder";
const char *BLE_DEVICE_NAME = BLE_LOCAL_NAME;
const char *BLE_STEPPER_SERVICE_ID = "19b10000-e8f2-537e-4f6c-d104768a1214";
const char *BLE_SPEED_CHAR_ID = "19b10001-e8f2-537e-4f6c-d104768a1214";
const char *BLE_PRESSURE_TARGET_CHAR_ID = "b9ec56a5-b66c-4ec6-89e2-7d539d606171";
const char *BLE_PRESSURE_SENSOR_CHAR_ID = "991a50fa-b15c-434c-877d-9026a65363cb";

const int fullStepRev = 200;
const int stepFrag = 5;
const int stepsPerRevolution = fullStepRev * stepFrag; // change this to fit the number of steps per revolution

int speed = 0; // initial speed is 0

// initialize the stepper library on pins 8, 9:
Stepper pumpStepper(stepsPerRevolution, 8, 9);


BLEService StepperService(BLE_STEPPER_SERVICE_ID); // BLE LED Service
BLEIntCharacteristic speedCharacteristic(BLE_SPEED_CHAR_ID, BLERead | BLEWrite | BLENotify);

void setup()
{
  // set the speed at 60 rpm:
  // initialize the serial port:
  Serial.begin(9600);
  // while (!Serial);

  pinMode(LED_BUILTIN, OUTPUT);

  if (!BLE.begin())
  {
    Serial.println("Starting BLE failed!");

    while (1);
  }

  // set advertised local name and service UUID:
  BLE.setLocalName(BLE_LOCAL_NAME);
  BLE.setDeviceName(BLE_DEVICE_NAME);
  BLE.setAdvertisedService(StepperService);

  // add the characteristic to the service
  StepperService.addCharacteristic(speedCharacteristic);

  // add service
  BLE.addService(StepperService);

  // set the initial value for the characeristic:
  speedCharacteristic.writeValue(speed);

  // start advertising
  BLE.advertise();

  Serial.println("Stepper Control BLE Peripheral");
}

bool BLEStatus = false;

void loop()
{
  // listen for BLE peripherals to connect:
  BLEDevice central = BLE.central();
  // Serial.println("waiting for central");

  // if a central is connected to peripheral:
  if (central.connected())
  {
    if(BLEStatus == false) {
      digitalWrite(LED_BUILTIN, HIGH); // turn the LED on (HIGH is the voltage level)
      Serial.print("Connected to central: ");
      // print the central's MAC address:
      Serial.println(central.address());
      BLEStatus = true;
    }

    // check for characteristic write
    if (speedCharacteristic.written())
    {
      Serial.print("Speed: ");
      Serial.println(speedCharacteristic.value(), 10);
      speed = speedCharacteristic.value();

      pumpStepper.setMove(0, speed);
    }

    
  }

  // do step if required
  pumpStepper.step();

  if (!central.connected() && BLEStatus == true)
  {
    // write the speed setting to BLE so that on reconnect the central can read it
    speedCharacteristic.writeValue(speed);
    digitalWrite(LED_BUILTIN, LOW); // turn the LED off by making the voltage LOW
    Serial.print(F("Disconnected from central: "));
    Serial.println(central.address());
    BLEStatus = false;
  }
}
