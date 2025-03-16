#ifndef BLUETOOTH_COMM_H
#define BLUETOOTH_COMM_H

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

#define DEVICE_NAME                 "Wheelchair Angle Monitor"
#define SERVICE_UUID                "45941ccb-b601-4b21-b5c4-37998c0bdc47"
#define ANGLE_CHARACTERISTIC_UUID   "f1919650-bf45-4adb-b449-7ad7e3cdf038"
#define STATUS_CHARACTERISTIC_UUID    "1e89f4d9-2e15-4d60-9ee9-0308c1c8c90e"
#define OTHER_CHARACTERISTIC_UUID   "2cc7e495-4db7-41ef-b805-bbf9cf87e96c"

// Initialization function
void initBLE();

// Globals for each the parts of the BLE server
BLEServer *pServer;
BLEService *pService;
BLECharacteristic *pAngleCharacteristic;
BLECharacteristic *pStatusCharacteristic;
BLECharacteristic *pOtherCharacteristic;
BLEAdvertising *pAdvertising;

// Global variable to track whether or not we are connected
uint8_t connectedBLE = 0;

#endif