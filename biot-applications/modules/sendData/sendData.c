#include <stdio.h>
#include <xtimer.h>

#include <arpa/inet.h>
#include "net/ipv6.h"

#include "sendData.h"
#include "../biotUdp/udp.h"

void sendData(char *address, nodeData_t data)
{
    char buffer[42];
    sprintf(buffer, "data:%lu:%f:%f:%f:%f", data.timeStamp, data.w, data.x, data.y, data.z);
    udpSend(address, buffer);
}

void sendCalibration(char *address, int16_t *cal)
{
    char buffer[42];
    sprintf(buffer, "calib:%d:%d:%d:%d:%d:%d", cal[0], cal[1], cal[2], cal[3], cal[4], cal[5]);
    udpSend(address, buffer);
}

