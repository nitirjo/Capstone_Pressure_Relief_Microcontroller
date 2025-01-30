#include "BluetoothComm.h"
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <String.h>
#include "Values.h"

class AngleCharacteristicCallbacks: public BLECharacteristicCallbacks {
  // When the Angle Characteristic is read, it should
  // update the its own angle values
  void onRead(BLECharacteristic* pCharacteristic) {
    // This will contain alternating roll/pitch
    // float sensorReadings[SENSOR_COUNT*2];

    union {
      float float_arr[SENSOR_COUNT*2];
      uint8_t byte_arr[SENSOR_COUNT*2*sizeof(float)];
    } sensorReadings;

    for (int i = 0; i < SENSOR_COUNT; i++) {
      sensorReadings.float_arr[i*2] = angleValues[i*2];
      sensorReadings.float_arr[i*2+1] = angleValues[i*2+1];
    }

    // Calculate size of data
    int dataSize = sizeof(float) * SENSOR_COUNT * 2;

    Serial.println("---");
    for(int i = 0; i < SENSOR_COUNT*2; i++) {
      Serial.print(sensorReadings.float_arr[i]);
      Serial.print(" : ");
      Serial.print(sensorReadings.byte_arr[i*4+0],HEX);
      Serial.print(sensorReadings.byte_arr[i*4+1],HEX);
      Serial.print(sensorReadings.byte_arr[i*4+2],HEX);
      Serial.print(sensorReadings.byte_arr[i*4+3],HEX);
      Serial.println();
    }

    pCharacteristic->setValue(sensorReadings.byte_arr, dataSize);

    for(int i = 0; i < SENSOR_COUNT; i++) {
      angleValues[i] = angleValues[i] + 1;
    }
  }
};

class StatusCharacteristicCallbacks: public BLECharacteristicCallbacks {
  // On read, update this with an array of sensor statuses
  void onRead(BLECharacteristic* pCharacteristic) {
    uint8_t bytes[] = {0, 1, 1};
    pCharacteristic->setValue(bytes, 3);
  }
};

class OtherCharaceristicCallbacks: public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic* pCharacteristic) {
    Serial.println("Someone wrote to a characteristic! WOW!\n");
    String r = pCharacteristic->getValue();
    Serial.println(r);
  }
};

/**
  Initiatialize BLE-related parts of the MCU
  Includes the server, service, characteristics, and their original values
  Also starts the service and advertises
*/
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

  pStatusCharacteristic =
    pService->createCharacteristic(STATUS_CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
  pStatusCharacteristic->setValue("default status");

  pOtherCharacteristic =
    pService->createCharacteristic(OTHER_CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
  pOtherCharacteristic->setValue("default other");

  // Set callbacks for characteristics
  pAngleCharacteristic->setCallbacks(new AngleCharacteristicCallbacks());
  pStatusCharacteristic->setCallbacks(new StatusCharacteristicCallbacks());

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