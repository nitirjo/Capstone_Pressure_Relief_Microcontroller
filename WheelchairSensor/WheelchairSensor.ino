#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <sstream>
#include "BluetoothComm.h"
#include "IMUSensors.h"

#define DEFAULT_DSO32 0x6A
#define MUX_ADDR 0x70

#define SENSOR_COUNT 2

//struct IMUSensor sensor;
struct IMUSensor sensors[SENSOR_COUNT];

void setup() {
  Serial.begin(9600);
  initMUX();
  initBLE();

  //sensors = (struct IMUSensor*) malloc(SENSOR_COUNT*sizeof(struct IMUSensor));
  for (int i = 0; i < SENSOR_COUNT; i++) {
    initIMU(&sensors[i], DEFAULT_DSO32, i);
    calibrateIMU(&sensors[i]);
  }
}

void loop() {
  //Serial.printf("I am not dead %d\n", aliveCounter);
  
  // IMU stuff
  for (int i = 0; i < SENSOR_COUNT; i++) {
    readIMU(&sensors[i]);
    calculateAngles(&sensors[i]);
  }

  if (aliveCounter%10==0) {
    printStuff();
    
    std::stringstream ss;
    ss << "R" << sensors[0].roll << ", P" << sensors[0].pitch;
    std::string str = ss.str();
    const char* cstr = str.c_str();
    pAngleCharacteristic->setValue(cstr);
  }

  delay(15);
  aliveCounter++;
}

void printStuff() {
  for (int i = 0; i < SENSOR_COUNT; i++) {
    if (!sensors[i].connected) {
      Serial.printf("---- Sensor %d Not Connected ----\n\n", i);
    } else {

      Serial.printf("---- SENSOR %d ----\n", i);
      
      /* Display the results (acceleration is measured in m/s^2) */
      Serial.print("\t\tAccel X: ");
      Serial.print(sensors[i].accel.acceleration.x + sensors[i].accOffsetX);
      Serial.print(" \tY: ");
      Serial.print(sensors[i].accel.acceleration.y + sensors[i].accOffsetY);
      Serial.print(" \tZ: ");
      Serial.print(sensors[i].accel.acceleration.z + sensors[i].accOffsetZ);
      Serial.println(" m/s^2 ");

      /* Display the results (rotation is measured in rad/s) */
      Serial.print("\t\tGyro X: ");
      Serial.print(sensors[i].gyro.gyro.x);
      Serial.print(" \tY: ");
      Serial.print(sensors[i].gyro.gyro.y);
      Serial.print(" \tZ: ");
      Serial.print(sensors[i].gyro.gyro.z);
      Serial.println(" radians/s ");
      Serial.println();

      /* Display pitch and roll */
      Serial.print("\t\tRoll:  ");
      Serial.print(sensors[i].roll);
      Serial.println(" degrees");
      Serial.print("\t\tPitch: ");
      Serial.print(sensors[i].pitch);
      Serial.println(" degrees");
      Serial.println();
    }
  }
}
