#include <stdio.h>
#include <xtimer.h>

#include "net/ipv6.h"
#include <arpa/inet.h>

int32_t timeCorrection = 0;
int32_t prevTimeCorrection = 0;
bool timeSet = false;


uint32_t getCurrentTime(void)
{
    return xtimer_now() - timeCorrection;
}

void setCurrentTime(uint32_t t)
{
    prevTimeCorrection = timeCorrection;
    timeCorrection = xtimer_now() - t;
    timeSet = true;
    printf("time correction now: %lu\n", timeCorrection);
}

bool isTimeSet(void)
{
    return timeSet;
}

void timeInit(void)
{
    timeSet = false;
    timeCorrection = 0;
}

bool hasTimeChanged(void)
{
    if ((int64_t)(prevTimeCorrection/10000) == (int64_t)(timeCorrection/10000))
        return false;
    prevTimeCorrection = timeCorrection;
    return true;
}
