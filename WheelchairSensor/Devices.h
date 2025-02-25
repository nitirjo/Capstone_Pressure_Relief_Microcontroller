#ifndef DEVICES_H
#define DEVICES_H

#define DEFAULT_DSO32 0x6A
#define MUX_ADDR 0x70

#define SENSOR_COUNT 3

#define FILTER_UPDATE_RATE_HZ 100

//struct IMUSensor sensor
struct IMUSensor sensors[SENSOR_COUNT];

#endif