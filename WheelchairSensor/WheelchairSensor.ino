/*
  Debugging definitions
*/
//#define DBG_RDNGS         // define to print any kind of reading
//#define DBG_ANGLE_RDNG    // define to print the angle in a easy-to-read format
//#define DBG_STATUS_RDNG   // define to print out each sensor's connected status
//#define DBG_ANGLE_CSV     // define to print angles in csv format for testing
//#define DBG_ANGLE_RDNG    // define to print the angle in a easy-to-read format
//#define DBG_STATUS_RDNG   // define to print out each sensor's connected status
//#define DBG_ANGLE_CSV     // define to print angles in csv format for testing
//#define DBG_RDNGS_RAW 0   // define to print out the sensor's raw readings

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <sstream>
#include <Adafruit_AHRS.h>
#include "IMUSensors.h"
#include "Devices.h"

uint8_t aliveCounter = 0;
uint32_t noConnectCount = 0;
uint32_t timestamp;

void setup() {
  Serial.begin(115200);
  // Initialize BLE server
  initBLE();

  // Initialize I2C MUX.
  initMUX();

  // Initialize all IMU sensors
  for (int i = 0; i < SENSOR_COUNT; i++) {
    initIMU(&sensors[i], i+1);
    calibrateIMU(&sensors[i]);
  }

  // Get initial value of timestamp for managing update rate
  timestamp = millis();

  // Don't print this in csv mode, just so the output is cleaner
  #ifndef DBG_ANGLE_CSV
  Serial.println("STARTED");
  #endif
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
    Serial.printf("%f  ", sensors[i].currRoll);
  }
  Serial.print("\n");

  Serial.print("Pitch:  ");
  for (int i = 0; i < SENSOR_COUNT; i++) {
    Serial.printf("%f  ", sensors[i].currPitch);
  }
  Serial.print("\n");

  Serial.print("Yaw:  ");
  for (int i = 0; i < SENSOR_COUNT; i++) {
    Serial.printf("%f  ", sensors[i].currYaw);
  }
  Serial.print("\n");
  #endif

  // Print out whether each sensor is connected
  #ifdef DBG_STATUS_RDNG
  for (int i = 0; i < SENSOR_COUNT; i++) {
    if (sensors[i].connected) {
      Serial.printf("Sensor %d: ON     ", i);
    } else {
      Serial.printf("Sensor %d: OFF    ", i);
    }
    Serial.printf("\n");
  }
  #endif

  // Print out the angles in CSV format
  // This only prints out the roll and pitch
  #ifdef DBG_ANGLE_CSV
    for (int i = 0; i < SENSOR_COUNT-1; i++) {
    Serial.printf("%f,%f,", sensors[i].currRoll, sensors[i].currPitch);
  }
  Serial.printf("%f,%f\n", sensors[SENSOR_COUNT-1].currRoll, sensors[SENSOR_COUNT-1].currPitch);
  #endif

  #ifdef DBG_RDNGS_RAW
  Serial.printf("Sensor DBG_RDNGS_RAW:\n");
  Serial.printf("\tAcc: x: %f\ty: %f\tz: %f\n", sensors[DBG_RDNGS_RAW].accel.acceleration.x, 
      sensors[DBG_RDNGS_RAW].accel.acceleration.y, sensors[DBG_RDNGS_RAW].accel.acceleration.z);
  Serial.printf("\tGyr: x: %f\ty: %f\tz: %f\n", sensors[DBG_RDNGS_RAW].gyro.gyro.x, 
      sensors[DBG_RDNGS_RAW].gyro.gyro.y, sensors[DBG_RDNGS_RAW].gyro.gyro.z);
  Serial.printf("\tMag: x: %f\ty: %f\tz: %f\n", sensors[DBG_RDNGS_RAW].mag.magnetic.x, 
      sensors[DBG_RDNGS_RAW].mag.magnetic.y, sensors[DBG_RDNGS_RAW].mag.magnetic.z);    
  #endif
}
