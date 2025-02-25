#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <sstream>
#include <Adafruit_AHRS.h>
#include "IMUSensors.h"
#include "Devices.h"

uint8_t aliveCounter = 0;
uint32_t timestamp;

void setup() {
  Serial.begin(115200);
  initMUX();

  // Initialize all IMU sensors
  for (int i = 0; i < SENSOR_COUNT; i++) {
    initIMU(&sensors[i], i);
    calibrateIMU(&sensors[i]);
  }

  // Get initial value of timestamp for managing update rate
  timestamp = millis();

  Serial.println("STARTED");
}

void loop() {
  // Filter update rate
  if ((millis() - timestamp) < (1000 / FILTER_UPDATE_RATE_HZ)) {
    return;
  }
  timestamp = millis();

  // Update IMU and calculated values
  for (int i = 0; i < SENSOR_COUNT; i++) {
    readIMU(&sensors[i]);
    calculateAngles(&sensors[i]);
  }

  if (aliveCounter%10==0) {
    printStuff();
  }

  aliveCounter++;
}


/**
  Print data to serial. For testing purposes
*/
void printStuff() {  
  Serial.print("Roll:  ");
  for (int i = 0; i < SENSOR_COUNT; i++) {
    Serial.printf("%f  ", sensors[i].filter.getRoll());
  }
  Serial.print("\n");

  Serial.print("Pitch:  ");
  for (int i = 0; i < SENSOR_COUNT; i++) {
    Serial.printf("%f  ", sensors[i].filter.getPitch());
  }
  Serial.print("\n");

  Serial.print("Yaw:  ");
  for (int i = 0; i < SENSOR_COUNT; i++) {
    Serial.printf("%f  ", sensors[i].filter.getYaw());
  }
  Serial.print("\n");
}
