#include <stdio.h>
#include <xtimer.h>

#include <arpa/inet.h>
#include "net/ipv6.h"

#include "sendData.h"
#include "../biotUdp/udp.h"

void sendData(char *address, nodeData_t data)
{
    char buffer[42];
    sprintf(buffer, "do#%lu:%f:%f:%f:%f", data.timeStamp, data.w, data.x, data.y, data.z);
    udpSend(address, buffer);
}

void sendCalibration(char *address, int16_t *cal)
{
    char buffer[42];
    sprintf(buffer, "dc#%d:%d:%d:%d:%d:%d", cal[0], cal[1], cal[2], cal[3], cal[4], cal[5]);
    udpSend(address, buffer);
}

void translateStatus(imuStatus_t *status, char* buffer)
{
    uint8_t dof = 0;
    if (status->useGyroscopes)
        dof += 100;
    if (status->useAccelerometers)
        dof += 10;
    if (status->useMagnetometers)
        dof += 1;

    uint8_t mode = 0;
    if (status->calibrateMode)
        mode = 1;

    sprintf(buffer, "ds#%d:%"SCNu32":%d", dof, status->dupInterval, mode);
}

void sendStatus(char *address, imuStatus_t *status)
{
    char buffer[42];
    translateStatus(status, buffer);
    udpSend(address, buffer);
}

