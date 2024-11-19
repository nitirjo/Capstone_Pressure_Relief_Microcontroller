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

  std::stringstream s1;
  s1 << "Angle alive: " << aliveCounter;
  std::string str1 = s1.str();
  const char* cstr1 = str1.c_str();
  pAngleCharacteristic->setValue(cstr1);

  std::stringstream s2;
  s2 << "Time alive: " << aliveCounter;
  std::string str2 = s2.str();
  const char* cstr2 = str2.c_str();
  pTimeCharacteristic->setValue(cstr2);

  std::stringstream s3;
  s3 << "Other alive: " << aliveCounter;
  std::string str3 = s3.str();
  const char* cstr3 = str3.c_str();
  pOtherCharacteristic->setValue(cstr3);

  delay(1000);
  aliveCounter++;
}