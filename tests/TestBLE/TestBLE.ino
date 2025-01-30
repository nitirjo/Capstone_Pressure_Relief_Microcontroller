#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <sstream>
#include "BluetoothComm.h"

// Gyroscope tries to reset to accelerometer values
// everything GYRO_RESET_FREQ cycles (MAX 255)
#define GYRO_RESET_FREQ 100

void setup() {
  initBLE();

  Serial.begin(9600);
  Serial.println("STARTED");
}

void loop() {

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
  Serial.print("I'm alive ");
  Serial.println(aliveCounter);
}
