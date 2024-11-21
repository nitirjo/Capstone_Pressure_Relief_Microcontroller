#include <Adafruit_LSM6DSO32.h>
#include <math.h>
#include <Wire.h>
#include "IMUSensors.h"

// Initialize the imu sensor struct
int initIMU(struct IMUSensor *sensor, uint8_t addr, uint8_t id) {
  sensor->connected = sensor->imu.begin_I2C(addr, &Wire, id);
  
  // If sensor not found, quit
  if (!sensor->connected) {
    return sensor->connected;
  }

  // Sensor settings
  sensor->imu.setAccelRange(LSM6DSO32_ACCEL_RANGE_8_G);
  sensor->imu.setGyroRange(LSM6DS_GYRO_RANGE_250_DPS );
  sensor->imu.setAccelDataRate(LSM6DS_RATE_12_5_HZ);
  sensor->imu.setGyroDataRate(LSM6DS_RATE_12_5_HZ);

  // Initialize member variables for calculating pitch, roll
  sensor->previousTime = 0;
  sensor->currentTime = millis();
  sensor->elapsedTime = 0;
  sensor->accAngleX = 0;
  sensor->accAngleY = 0;
  sensor->gyroAngleX = 0;
  sensor->gyroAngleY = 0;

  // Set ID
  sensor->id = id;

  // TODO: calibration routine. may want to break out into separate function
  calibrateIMU(sensor);

  return sensor->connected;
}

// Updates the sensor variables in the sensor struct
// Returns 1 if the sensor is not connected
// Returns 0 otherwise
int readIMU(struct IMUSensor *sensor) {
  // Quit early if the sensor is not connected
  if(!sensor->connected) {
    return 1;
  }

  // Update values
  sensor->imu.getEvent(&(sensor->accel), &(sensor->gyro), &(sensor->temp));
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

  // Grab accelerometer values
  double ax = sensor->accel.acceleration.x + sensor->accOffsetX;
  double ay = sensor->accel.acceleration.y + sensor->accOffsetY;
  double az = sensor->accel.acceleration.z + sensor->accOffsetZ;

  // Grab gyroscope values. Round to nearest hundredth to prevent drift
  double gx = (round(100.0 * (sensor->gyro.gyro.x + sensor->gyroOffsetX)) / 100.0);
  double gy = (round(100.0 * (sensor->gyro.gyro.y + sensor->gyroOffsetY)) / 100.0);
  double gz = (round(100.0 * (sensor->gyro.gyro.z + sensor->gyroOffsetZ)) / 100.0);
  
  // Calculate angle as based on accelerometer data
  sensor->accAngleX = atan(ay / sqrt(pow(ax, 2) + pow(az, 2))) * (180 / PI);
  sensor->accAngleY = atan(-1 * ax / sqrt(pow(ay, 2) + pow(az, 2))) * (180 / PI);

  // Find the time elapsed since previous cycle to calculate change in gyroscope angle
  sensor->previousTime = sensor->currentTime;
  sensor->currentTime = millis();
  sensor->elapsedTime = (sensor->currentTime - sensor->previousTime) / 1000.0;

  // Calculate angle as based on gyro data. Calculates by adding change in angle to previous value
  sensor->gyroAngleX = sensor->gyroAngleX + gx * sensor->elapsedTime;
  sensor->gyroAngleY = sensor->gyroAngleY + gy * sensor->elapsedTime;

  // Calculate final roll (x) and pitch (y) as the weighted average of accelerometer and gyro measurements
  sensor->roll = (sensor->gyroAngleX * 180.0 / PI) * GYRO_WEIGHT + sensor->accAngleX * ACC_WEIGHT;
  sensor->pitch = (sensor->gyroAngleY * 180.0 / PI) * GYRO_WEIGHT + sensor->accAngleY * ACC_WEIGHT;

  return 0;
}

// Sets offset values of IMU, assumes that the IMU
// is oriented correctly and at rest
void calibrateIMU(struct IMUSensor *sensor) {
  // Update values in IMU for calibration
  readIMU(sensor);

  // Calculate accelerometer offsets
  sensor->accOffsetX = DEFAULT_AX - sensor->accel.acceleration.x;
  sensor->accOffsetY = DEFAULT_AY - sensor->accel.acceleration.y;
  sensor->accOffsetZ = DEFAULT_AZ - sensor->accel.acceleration.z;

  // ignore us :)
  sensor->gyroOffsetX = 0;
  sensor->gyroOffsetY = 0;
  sensor->gyroOffsetZ = 0;
}