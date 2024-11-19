#include "BluetoothComm.h"
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

void initBLE() {
  // Init device
  BLEDevice::init(DEVICE_NAME);

  // Get server object
  pServer = BLEDevice::createServer();
  
  // Get service object under server
  pService = pServer->createService(SERVICE_UUID);
  
  // Get characteristics under service.  Set default values
  pAngleCharacteristic =
    pService->createCharacteristic(ANGLE_CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
  pAngleCharacteristic->setValue("default angle");

  pTimeCharacteristic =
    pService->createCharacteristic(TIME_CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
  pTimeCharacteristic->setValue("default time");

  pOtherCharacteristic =
    pService->createCharacteristic(OTHER_CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
  pOtherCharacteristic->setValue("default other");

  // Start service
  pService->start();

  // BLEAdvertising *pAdvertising = pServer->getAdvertising();  // this still is working for backward compatibility
  pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
}