#ifndef DEVICES_H
#define DEVICES_H

#define DEFAULT_DSO32 0x6B
#define MUX_ADDR 0x70

#define SENSOR_COUNT 3

#define FILTER_UPDATE_RATE_HZ 100

#define WHO_AM_I_REG 0x0F
#define WHO_AM_I 0x68

struct IMUSensor sensors[SENSOR_COUNT];

#endif