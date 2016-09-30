#ifndef _SEND_DATA_H_
#define _SEND_DATA_H_

#include "../imu/imu.h"

typedef struct nodeDataStructure {
    uint32_t timeStamp;
    double w;    
    double x;    
    double y;    
    double z;    
} nodeData_t;

void sendCalibration(char *address, int16_t *cal);

void sendData(char *address, nodeData_t data);

void sendStatus(char *address, imuStatus_t *status);

void translateStatus(imuStatus_t *status, char *buffer);

#endif
