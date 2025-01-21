#include <Adafruit_LSM6DSO32.h>
#include <math.h>
#include <Wire.h>
#include "Mux_Control.h"
#include "IMUSensors.h"

// Initialize the imu sensor struct
int initIMU(struct IMUSensor *sensor, uint8_t addr, uint8_t id) {
  // Set ID and SEIZE CONTROL! of the MUX
  sensor->id = id;
  selectMuxPort(id);
  
  sensor->connected = sensor->imu.begin_I2C(addr);
  
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

  // Calibrate the IMU
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

  // Set MUX port to this
  selectMuxPort(sensor->id);

  // Update values
  sensor->imu.getEvent(&(sensor->accel), &(sensor->gyro), &(sensor->temp));

  // Find the time elapsed since previous cycle to calculate change in gyroscope angle
  sensor->previousTime = sensor->currentTime;
  sensor->currentTime = millis();
  sensor->elapsedTime = (sensor->currentTime - sensor->previousTime) / 1000.0;

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
  // Average a bunch of measurements to get the offsets
  double avgAccX = 0;
  double avgAccY = 0;
  double avgAccZ = 0;
  double avgGyroX = 0;
  double avgGyroY = 0;
  double avgGyroZ = 0;

  for (int i = 0; i < CALIBRATION_CYCLES; i++) {
    // Update values
    readIMU(sensor);

    // Collect accelerometer values
    avgAccX += sensor->accel.acceleration.x;
    avgAccY += sensor->accel.acceleration.y;
    avgAccZ += sensor->accel.acceleration.z;
    avgGyroX += sensor->gyro.gyro.x;
    avgGyroY += sensor->gyro.gyro.y;
    avgGyroZ += sensor->gyro.gyro.z;

    delay(20);
  }

  // Get averages
  avgAccX /= CALIBRATION_CYCLES;
  avgAccY /= CALIBRATION_CYCLES;
  avgAccZ /= CALIBRATION_CYCLES;
  avgGyroX /= CALIBRATION_CYCLES;
  avgGyroY /= CALIBRATION_CYCLES;
  avgGyroZ /= CALIBRATION_CYCLES;

  // Set offsets based on averages
  sensor->accOffsetX = DEFAULT_AX - avgAccX;
  sensor->accOffsetY = DEFAULT_AY - avgAccY;
  sensor->accOffsetZ = DEFAULT_AZ - avgAccZ;
  // sensor->accOffsetX = 0;
  // sensor->accOffsetY = 0;
  // sensor->accOffsetZ = 0;
  sensor->gyroOffsetX = -avgGyroX;
  sensor->gyroOffsetY = -avgGyroY;
  sensor->gyroOffsetZ = -avgGyroZ;

  // Reset gyro angles
  sensor->gyroAngleX = 0;
  sensor->gyroAngleY = 0;
}

// Sets the angles calculated with gyroscopes to
// that of the accelerometers.
// This is to accomodate the drift that the gyros experience
void recenterIMU(struct IMUSensor *sensor) {
  // sensor->gyroAngleX = sensor->accAngleX;
  // sensor->gyroAngleY = sensor->accAngleY;
  if (sensor->accAngleX < ACC_ANGLE_LIMIT && sensor->accAngleX > -ACC_ANGLE_LIMIT) {
    sensor->gyroAngleX = 0;
  }

  if (sensor->accAngleY < ACC_ANGLE_LIMIT && sensor->accAngleY > -ACC_ANGLE_LIMIT) {
    sensor->gyroAngleY = 0;
  }
}