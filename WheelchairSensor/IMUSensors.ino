#include <math.h>
#include <Wire.h>

#include <Adafruit_LSM9DS1.h>
#include <Adafruit_Sensor_Calibration.h>
#include <Adafruit_AHRS.h>

#include "Mux_Control.h"
#include "IMUSensors.h"
#include "Devices.h"
#include "Calibrations.h"

// Initialize the imu sensor struct
int initIMU(struct IMUSensor *sensor, uint8_t id) {
  // Set ID and SEIZE CONTROL! of the MUX
  sensor->id = id;
  selectMuxPort(id);
  
  sensor->imu = Adafruit_LSM9DS1(); 
  sensor->connected = sensor->imu.begin();
  
  // If sensor not found, quit
  if (!sensor->connected) {
    return sensor->connected;
  }

  // Sensor settings
  sensor->imu.setupAccel(sensor->imu.LSM9DS1_ACCELRANGE_2G);
  sensor->imu.setupMag(sensor->imu.LSM9DS1_MAGGAIN_4GAUSS);
  sensor->imu.setupGyro(sensor->imu.LSM9DS1_GYROSCALE_245DPS);

  // Setup filter
  sensor->filter.begin(FILTER_UPDATE_RATE_HZ);

  return sensor->connected;
}

// Updates the sensor variables in the sensor struct
// Returns 1 if the sensor is not connected
// Returns 0 otherwise
int readIMU(struct IMUSensor *sensor) {
  //if (!sensor->connected) return 1;
  
  // Set MUX port to this
  selectMuxPort(sensor->id);

  // Check whether or not the sensor is connected
  Wire.beginTransmission(DEFAULT_DSO32);    // Get the slave's attention, tell it we're sending a command byte
  Wire.write(WHO_AM_I_REG);                               //  The command byte, sets pointer to register with address of 0x32
  Wire.endTransmission();
  Wire.requestFrom(DEFAULT_DSO32,1);          // Tell slave we need to read 1byte from the current register
  byte check = Wire.read();        // read that byte into 'slaveByte2' variable
  if (check != 0x68) {
    sensor->connected = 0;
  } else if (!sensor->connected) { // Re-init a previously disconnected IMU
    initIMU(&sensors[sensor->id], sensor->id);
  }

  // Update values
  sensor->imu.read();
  sensor->imu.getEvent(&(sensor->accel), &(sensor->mag), &(sensor->gyro), &(sensor->temp));

  return 0;
}

// Calculates the current roll and pitch
// Returns 1 if the sensor is not connected
// Returns 0 otherwise
int calculateAngles(struct IMUSensor *sensor) {
  // Quit early if the sensor is not connected
  if(!sensor->connected) {
    return 1;
  }

  // Grab gyroscope values. Apply calibration. Convert to degree/s
  float gx = (sensor->gyro.gyro.x - sensor->gyroXCalib) * (180/PI);
  float gy = (sensor->gyro.gyro.y - sensor->gyroYCalib) * (180/PI);
  float gz = (sensor->gyro.gyro.z - sensor->gyroZCalib) * (180/PI);  

  // Grab accelerometer values. Apply calibration
  float ax = sensor->accel.acceleration.x - sensor->accXCalib;
  float ay = sensor->accel.acceleration.y - sensor->accYCalib;
  float az = sensor->accel.acceleration.z - sensor->accZCalib;

  // Grab magnetometer values. Apply calibration
  float mx = sensor->mag.magnetic.x - sensor->magXCalib;
  float my = sensor->mag.magnetic.y - sensor->magYCalib;
  float mz = sensor->mag.magnetic.z - sensor->magZCalib;

  // Calculate angles
  sensor->filter.update(gx, gy, gz, ax, ay, az, mx, my, mz);

  sensor->currPitch = sensor->filter.getPitch();
  sensor->currRoll = sensor->filter.getRoll();
  sensor->currYaw = sensor->filter.getYaw();

  return 0;
}

// Loads in offsets from the header file. Only works for 
// sensors with id's 0-2
void calibrateIMU(struct IMUSensor *sensor) {
  if (sensor->id == 0) {
    sensor->gyroXCalib = GYRO0_X_CALIB;
    sensor->gyroYCalib = GYRO0_X_CALIB;
    sensor->gyroZCalib = GYRO0_X_CALIB;
    sensor->accXCalib = ACC0_X_CALIB;
    sensor->accYCalib = ACC0_X_CALIB;
    sensor->accZCalib = ACC0_X_CALIB;
    sensor->magXCalib = MAG0_X_CALIB;
    sensor->magYCalib = MAG0_X_CALIB;
    sensor->magZCalib = MAG0_X_CALIB;
  } else if (sensor->id == 1) {
    sensor->gyroXCalib = GYRO1_X_CALIB;
    sensor->gyroYCalib = GYRO1_X_CALIB;
    sensor->gyroZCalib = GYRO1_X_CALIB;
    sensor->accXCalib = ACC1_X_CALIB;
    sensor->accYCalib = ACC1_X_CALIB;
    sensor->accZCalib = ACC1_X_CALIB;
    sensor->magXCalib = MAG1_X_CALIB;
    sensor->magYCalib = MAG1_X_CALIB;
    sensor->magZCalib = MAG1_X_CALIB;
  } else if (sensor->id == 2) {
    sensor->gyroXCalib = GYRO2_X_CALIB;
    sensor->gyroYCalib = GYRO2_X_CALIB;
    sensor->gyroZCalib = GYRO2_X_CALIB;
    sensor->accXCalib = ACC2_X_CALIB;
    sensor->accYCalib = ACC2_X_CALIB;
    sensor->accZCalib = ACC2_X_CALIB;
    sensor->magXCalib = MAG2_X_CALIB;
    sensor->magYCalib = MAG2_X_CALIB;
    sensor->magZCalib = MAG2_X_CALIB;
  } else {
    sensor->gyroXCalib = 0;
    sensor->gyroYCalib = 0;
    sensor->gyroZCalib = 0;
    sensor->accXCalib = 0;
    sensor->accYCalib = 0;
    sensor->accZCalib = 0;
    sensor->magXCalib = 0;
    sensor->magYCalib = 0;
    sensor->magZCalib = 0;
  }

  return;
}