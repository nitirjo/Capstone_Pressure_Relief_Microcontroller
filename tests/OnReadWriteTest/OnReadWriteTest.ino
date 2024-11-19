/*
  This sketchbook is to test the onRead and onWrite functionality.
  As on 11/19/2024, the actual code for the microcontroller doesn't
  use it, but it may be useful in the future for further optimizing

  The data in the angle characteristic will continuously update.
  The data in the time characteristic will update only on read and send a message to serial.
  If data is written to the other characteristic, it will echo it to serial.
*/

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <sstream>
#include "BluetoothComm.h"

#define SENSOR_ADDR_1 0x70;

void setup() {
  Serial.begin(9600);  
  initBLE();
}

void loop() {
  Serial.printf("I am not dead %d\n", aliveCounter);

  // Continuously update the angle characteristic
  std::stringstream s1;
  s1 << "Angle alive: " << aliveCounter;
  std::string str1 = s1.str();
  const char* cstr1 = str1.c_str();
  pAngleCharacteristic->setValue(cstr1);

  std::stringstream s3;
  s3 << "Other alive: " << aliveCounter;
  std::string str3 = s3.str();
  const char* cstr3 = str3.c_str();
  pOtherCharacteristic->setValue(cstr3);

  delay(1000);
  aliveCounter++;
}