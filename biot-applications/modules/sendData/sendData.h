#ifndef _SEND_DATA_H_
#define _SEND_DATA_H_

typedef struct nodeDataStructure {
    uint32_t timeStamp;
    float w;    
    float x;    
    float y;    
    float z;    
} nodeData_t;

void sendCalibration(char *address, int16_t *cal);

void sendData(char *address, nodeData_t data);

#endif
