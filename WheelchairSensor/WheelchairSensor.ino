/*
  Debugging definitions
*/
//#define DBG_RDNGS
//#define DBG_ANGLE_RDNG

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

  #ifdef DBG_RDNGS
  if (aliveCounter%10==0) {
    printStuff();
  }
  #endif

  aliveCounter++;
}


/**
  Print data to serial. For testing purposes
*/
void printStuff() {  
  // Print out angle readings for roll, pitch, yaw
  #ifdef DBG_ANGLE_RDNG
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
  #endif

  // Print out whether each sensor is connected
  #ifdef DBG_STATUS_RDNG
  for (int i = 0; i < SENSOR_COUNT; i++) {
    if (sensors[i].connected) {
      Serial.printf("Sensor %d: ON", i);
    } else {
      Serial.printf("Sensor %d: OFF", i);
    }
  }
  #endif

  // Print out the angles in CSV format
  // This only prints out the roll and pitch
  #ifdef DBG_ANGLE_CSV
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
  #endif
}
