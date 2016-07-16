#include <stdio.h>
#include <inttypes.h>

#include "mpu9150.h"
#include "xtimer.h"
//#include "board.h"

#include "imu.h"

#define SLEEP   (10 * 1000u)

int main(int argc, char **argv)
{

    mpu9150_t dev;
    imuData_t imuData;

    if (initialiseIMU(&dev))
    {
        displayConfiguration(dev);
        while (1) {
            if (getIMUData(dev, &imuData))
            {
                displayData(imuData);
            }
            xtimer_usleep(SLEEP);
        }
    }
    return 0;
}
