#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <sstream>
#include "BluetoothComm.h"
#include "IMUSensors.h"

#define DEFAULT_DSO32 0x6A
#define MUX_ADDR 0x70

struct IMUSensor sensor;

void setup() {
  Serial.begin(9600);
  initMUX();  
  enableMuxPort(0);
  initBLE();
  initIMU(&sensor, DEFAULT_DSO32, 0);
  calibrateIMU(&sensor);
}

void loop() {
  //Serial.printf("I am not dead %d\n", aliveCounter);
  
  // IMU stuff
  readIMU(&sensor);
  calculateAngles(&sensor);

  if (aliveCounter%10==0) {
    printStuff();
    
    std::stringstream ss;
    ss << "R" << sensor.roll << ", P" << sensor.pitch;
    std::string str = ss.str();
    const char* cstr = str.c_str();
    pAngleCharacteristic->setValue(cstr);
  }

  delay(15);
  aliveCounter++;
}

void printStuff() {
  if (!sensor.connected) {
    Serial.printf("Sensor not connected!\n");
  }

  Serial.print("\t\tTemperature ");
  Serial.print(sensor.temp.temperature);
  Serial.println(" deg C");

  /* Display the results (acceleration is measured in m/s^2) */
  Serial.print("\t\tAccel X: ");
  Serial.print(sensor.accel.acceleration.x + sensor.accOffsetX);
  Serial.print(" \tY: ");
  Serial.print(sensor.accel.acceleration.y + sensor.accOffsetY);
  Serial.print(" \tZ: ");
  Serial.print(sensor.accel.acceleration.z + sensor.accOffsetZ);
  Serial.println(" m/s^2 ");

  /* Display the results (rotation is measured in rad/s) */
  Serial.print("\t\tGyro X: ");
  Serial.print(sensor.gyro.gyro.x);
  Serial.print(" \tY: ");
  Serial.print(sensor.gyro.gyro.y);
  Serial.print(" \tZ: ");
  Serial.print(sensor.gyro.gyro.z);
  Serial.println(" radians/s ");
  Serial.println();

  /* Display pitch and roll */
  Serial.print("\t\tRoll:  ");
  Serial.print(sensor.roll);
  Serial.println(" degrees");
  Serial.print("\t\tPitch: ");
  Serial.print(sensor.pitch);
  Serial.println(" degrees");
  Serial.println();
}
