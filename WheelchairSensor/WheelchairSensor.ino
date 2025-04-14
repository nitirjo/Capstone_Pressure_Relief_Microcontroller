#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <sstream>
#include "BluetoothComm.h"
#include "IMUSensors.h"
#include "Devices.h"

// Gyroscope tries to reset to accelerometer values
// everything GYRO_RESET_FREQ cycles (MAX 255)
#define GYRO_RESET_FREQ 100

void setup() {
  Serial.begin(9600);
  initMUX();
  initBLE();

  //sensors = (struct IMUSensor*) malloc(SENSOR_COUNT*sizeof(struct IMUSensor));
  for (int i = 0; i < SENSOR_COUNT; i++) {
    initIMU(&sensors[i], DEFAULT_DSO32, i+1);
    calibrateIMU(&sensors[i]);
  }

  Serial.println("STARTED");
}

void loop() {
  
  // IMU stuff
  for (int i = 0; i < SENSOR_COUNT; i++) {
    readIMU(&sensors[i]);
    calculateAngles(&sensors[i]);
  }

  // Every  cycles, recenter the IMUs
  if (aliveCounter==GYRO_RESET_FREQ) {
    for (int i = 0; i < SENSOR_COUNT; i++) {
      recenterIMU(&sensors[i]);
    }
  }

  if (aliveCounter%10==0) {
    printStuff();
  }

  delay(15);
  aliveCounter++;
}


/**
  Print data to serial. For testing purposes
*/
void printStuff() {
  for (int i = 0; i < SENSOR_COUNT-1; i++) {
    Serial.printf("%f,%f,",sensors[i].roll, sensors[i].pitch);
  }
  Serial.printf("%f,%f\n",sensors[SENSOR_COUNT-1].roll, sensors[SENSOR_COUNT-1].pitch);
}
