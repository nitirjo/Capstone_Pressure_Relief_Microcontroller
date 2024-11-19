#ifndef IMUSENSORS_H
#define IMUSENSORS_H

#include <Adafruit_LSM6DSO32.h>

#define PI 3.14159
#define GYRO_WEIGHT 1.0
#define ACC_WEIGHT 0.0

#define DEFAULT_AX 0.0
#define DEFAULT_AY 0.0
#define DEFAULT_AZ 9.8

struct IMUSensor {
  // Raw sensor events
  sensors_event_t accel;
  sensors_event_t gyro;
  sensors_event_t temp;

  // Values to help calculate roll, pitch
  float previousTime;
  float currentTime;
  float elapsedTime;
  float accAngleX;
  float accAngleY;
  float gyroAngleX;
  float gyroAngleY;

  // Accelerometer and gyroscope offsets
  float accOffsetX;
  float accOffsetY;
  float accOffsetZ;
  float gyroOffsetX;
  float gyroOffsetY;
  float gyroOffsetZ;

  // Calculated roll, pitch. In degrees
  float roll;
  float pitch;

  // Connected status and the IMU object
  int connected;
  Adafruit_LSM6DSO32 imu;
} ;

// Initialize the imu sensor struct
// Returns 1 if the sensor failed to connect
// Returns 0 otherwise
int initIMU(struct IMUSensor *sensor);

// Updates the sensor variables in the sensor struct
// Returns 1 if the sensor is not connected
// Returns 0 otherwise
int readIMU(struct IMUSensor *sensor);

// Sets offset values of IMU, assumes that the IMU
// is oriented flat and at rest
void calibrateIMU(struct IMUSensor *sensor);

// Calculates the current roll and pitch
// Returns 1 if the sensor is not connected
// Returns 0 otherwise
int calculateAngles(struct IMUSensor *sensor);

#endif 