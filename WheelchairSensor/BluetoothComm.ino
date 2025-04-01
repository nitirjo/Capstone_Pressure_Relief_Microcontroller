/*
  Debugging definitions
*/
//#define DEBUG_ONREAD

#include "BluetoothComm.h"
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include "Devices.h"
#include <String.h>

class AngleCharacteristicCallbacks: public BLECharacteristicCallbacks {
  // When the Angle Characteristic is read, it should
  // update the its own angle values
  void onRead(BLECharacteristic* pCharacteristic) {
    // This will contain alternating roll/pitch
    // float sensorReadings[SENSOR_COUNT*2];

    union {
      float float_arr[SENSOR_COUNT*2];
      uint8_t byte_arr[SENSOR_COUNT*2*sizeof(float)];
      uint32_t int_arr[SENSOR_COUNT*2];
    } sensorReadings;

    for (int i = 0; i < SENSOR_COUNT; i++) {
      sensorReadings.float_arr[i*2] = sensors[i].currRoll;
      sensorReadings.float_arr[i*2+1] = sensors[i].currPitch;
    }

    // Calculate size of data
    int dataSize = sizeof(float) * SENSOR_COUNT * 2;

    #ifdef DEBUG_ONREAD
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
    #endif

    pCharacteristic->setValue(sensorReadings.byte_arr, dataSize);
  }
};

class StatusCharacteristicCallbacks: public BLECharacteristicCallbacks {
  // On read, update this with an array of sensor statuses
  void onRead(BLECharacteristic* pCharacteristic) {
    uint8_t statuses[SENSOR_COUNT];
    
    for (int i = 0; i < SENSOR_COUNT; i++) {
      statuses[i] = sensors[i].connected;
    }

    pCharacteristic->setValue(statuses, SENSOR_COUNT);
  }
};

class OtherCharacteristicCallbacks: public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic* pCharacteristic) {
    Serial.println("Someone wrote to a characteristic! WOW!\n");
    String r = pCharacteristic->getValue();
    Serial.println(r);
  }
};

class DisReconnectCallbacks: public BLEServerCallbacks {
  void onDisconnect(BLEServer *pServer) {
    // This is a relatively safe if not inefficient way to 
    // let the app reconnect to the BLE server
    abort();
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
  pOtherCharacteristic->setCallbacks(new OtherCharacteristicCallbacks());

  // Set callback for disconnects
  pServer->setCallbacks(new DisReconnectCallbacks());

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