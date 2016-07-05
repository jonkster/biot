#ifndef _SEND_DATA_H_
#define _SEND_DATA_H_

typedef struct nodeDataStructure {
    uint32_t timeStamp;
    int16_t w;    
    int16_t x;    
    int16_t y;    
    int16_t z;    
} nodeData_t;

void sendData(char *address, nodeData_t data);

#endif
