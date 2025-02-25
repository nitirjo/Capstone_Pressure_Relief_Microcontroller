#include <math.h>
#include <Wire.h>

#include <Adafruit_LSM9DS1.h>
#include <Adafruit_Sensor_Calibration.h>
#include <Adafruit_AHRS.h>

#include "Mux_Control.h"
#include "IMUSensors.h"
#include "Devices.h"

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

  // Calibrate the IMU
  //calibrateIMU(sensor);

  return sensor->connected;
}

// Updates the sensor variables in the sensor struct
// Returns 1 if the sensor is not connected
// Returns 0 otherwise
int readIMU(struct IMUSensor *sensor) {
  // Quit early if the sensor is not connected
  if(!sensor->connected) {
    return 1;
    Serial.println("NOT CONNECTED");
  }

  // Set MUX port to this
  selectMuxPort(sensor->id);

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

  // Grab gyroscope values. Convert to degree/s
  float gx = sensor->gyro.gyro.x * (180/PI);
  float gy = sensor->gyro.gyro.y * (180/PI);
  float gz = sensor->gyro.gyro.z * (180/PI);  

  // Calculate angles
  sensor->filter.update(gx, gy, gz, 
                sensor->accel.acceleration.x, sensor->accel.acceleration.y, sensor->accel.acceleration.z, 
                sensor->mag.magnetic.x, sensor->mag.magnetic.y, sensor->mag.magnetic.z);

  return 0;
}

// Sets offset values of IMU, assumes that the IMU
// is oriented correctly and at rest
void calibrateIMU(struct IMUSensor *sensor) {
  // TODO
  return;
}

// Sets the angles calculated with gyroscopes to
// that of the accelerometers.
// This is to accomodate the drift that the gyros experience
void recenterIMU(struct IMUSensor *sensor) {
  // // sensor->gyroAngleX = sensor->accAngleX;
  // // sensor->gyroAngleY = sensor->accAngleY;
  // if (sensor->accAngleX < ACC_ANGLE_LIMIT && sensor->accAngleX > -ACC_ANGLE_LIMIT) {
  //   sensor->gyroAngleX = 0;
  // }

  // if (sensor->accAngleY < ACC_ANGLE_LIMIT && sensor->accAngleY > -ACC_ANGLE_LIMIT) {
  //   sensor->gyroAngleY = 0;
  // }

  // TODO
  return;
}