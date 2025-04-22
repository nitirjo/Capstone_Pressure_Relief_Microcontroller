#ifndef IMUSENSORS_H
#define IMUSENSORS_H

#include <Adafruit_LSM9DS1.h>
#include <Adafruit_Sensor_Calibration.h>
#include <Adafruit_AHRS.h>

#define PI 3.14159

struct IMUSensor {
  // Raw sensor events
  sensors_event_t accel;
  sensors_event_t gyro;
  sensors_event_t mag;
  sensors_event_t temp;

  // Sensor fusion filter
  Adafruit_NXPSensorFusion filter;

  // Calibration values
  float gyroXCalib, gyroYCalib, gyroZCalib;
  float accXCalib, accYCalib, accZCalib;
  float magXCalib, magYCalib, magZCalib;

  // Current values
  float currRoll, currPitch, currYaw;

  // Connected status and the IMU object
  uint8_t connected;
  uint8_t id;
  Adafruit_LSM9DS1 imu;
} ;

// Initialize the imu sensor struct
// Returns 1 if the sensor failed to connect
// Returns 0 otherwise
int initIMU(struct IMUSensor *sensor, uint8_t addr, uint8_t id);

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

// Sets the angles calculated with gyroscopes to
// that of the accelerometers.
// This is to accomodate the drift that the gyros experience
void recenterIMU(struct IMUSensor *sensor);

#endif 