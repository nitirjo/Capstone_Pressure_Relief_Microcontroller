#ifndef IMUSENSORS_H
#define IMUSENSORS_H

#include <Adafruit_LSM6DSO32.h>

#define PI 3.14159

// Weights for the weighted average of gyroscope and accelerometer
// angles. These should sum up to 1
#define GYRO_WEIGHT 0.9
#define ACC_WEIGHT 0.1

// Number of cycles to use for calibration
#define CALIBRATION_CYCLES 100

// Values to calibrate around for accelerometer
#define DEFAULT_AX 0.0
#define DEFAULT_AY 0.0
#define DEFAULT_AZ 9.8

// Will reset if within plus minus this angle
#define ACC_ANGLE_LIMIT 5

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
  uint8_t id;
  Adafruit_LSM6DSO32 imu;
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