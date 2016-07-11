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
    //puts(buffer);
    udp_send(address, buffer);
}

