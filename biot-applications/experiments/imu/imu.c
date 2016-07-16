#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <thread.h>
#include <xtimer.h>

#include "mpu9150.h"

#include "imu.h"

#define CHECK_BIT(var,pos) ((var) & (1<<(pos))) != 0

uint64_t t0;

uint16_t aFsrRange2Int(mpu9150_accel_ranges_t fsr)
{
    uint16_t rate = 0;
    switch(fsr)
    {
        case MPU9150_ACCEL_FSR_2G:
            rate = 2;
            break;
        case MPU9150_ACCEL_FSR_4G:
            rate = 4;
            break;
        case MPU9150_ACCEL_FSR_8G:
            rate = 8;
            break;
        case MPU9150_ACCEL_FSR_16G:
            rate = 16;
            break;
    }
    return rate;
}



void displayConfiguration(mpu9150_t dev)
{
    printf("+------------Configuration------------+\n");
    printf("Sample rate: %u Hz\n", dev.conf.sample_rate);
    printf("Compass sample rate: %u Hz\n", dev.conf.compass_sample_rate);
    printf("Accel full-scale range: %u G\n", aFsrRange2Int(dev.conf.accel_fsr));
    printf("Gyro full-scale range: %u DPS\n", gFsrRange2Int(dev.conf.gyro_fsr));
    printf("Compass X axis factory adjustment: %u\n", dev.conf.compass_x_adj);
    printf("Compass Y axis factory adjustment: %u\n", dev.conf.compass_y_adj);
    printf("Compass Z axis factory adjustment: %u\n", dev.conf.compass_z_adj);
    printf("+-------------------------------------+\n");
}

void displayData(imuData_t data)
{
    printf( "%"PRIu32", "
            "%"PRId16", %"PRId16", %"PRId16", "
            "%"PRId16", %"PRId16", %"PRId16", "
            "%"PRId16", %"PRId16", %"PRId16", "
            "%6.2f\n",
            data.ts,
            data.accel.x_axis, data.accel.y_axis, data.accel.z_axis,
            data.gyro.x_axis, data.gyro.y_axis, data.gyro.z_axis,
            data.mag.x_axis, data.mag.y_axis, data.mag.z_axis,
            data.temperature);
}

uint16_t gFsrRange2Int(mpu9150_gyro_ranges_t fsr)
{
    uint16_t rate = 0;
    switch(fsr)
    {
        case MPU9150_GYRO_FSR_250DPS:
            rate = 250;
            break;
        case MPU9150_GYRO_FSR_500DPS:
            rate = 500;
            break;
        case MPU9150_GYRO_FSR_1000DPS:
            rate = 1000;
            break;
        case MPU9150_GYRO_FSR_2000DPS:
            rate = 2000;
            break;
    }
    return rate;
}

bool getIMUData(mpu9150_t dev, imuData_t *data)
{
    data->ts = xtimer_now64() - t0;
    mpu9150_read_accel(&dev, &data->accel);
    mpu9150_read_gyro(&dev, &data->gyro);
    mpu9150_read_compass(&dev, &data->mag);
    int32_t rawTemp;
    mpu9150_read_temperature(&dev, &rawTemp);
    data->temperature = rawTemp/1000; // approx temperature in degrees C

    return true;
}

bool initialiseIMU(mpu9150_t *dev)
{
    int result;

    printf("+------------Initializing------------+\n");
    result = mpu9150_init(dev, I2C_0, MPU9150_HW_ADDR_HEX_68, MPU9150_COMP_ADDR_HEX_0C);

    if (result == -1) {
        puts("[Error] The given i2c is not enabled");
        return false;
    }
    else if (result == -2) {
        puts("[Error] The compass did not answer correctly on the given address");
        return false;
    }

    result = mpu9150_set_gyro_fsr(dev, GFSR);
    if (result == -1) {
        puts("[Error] The given i2c is not enabled");
        return false;
    }
    else if (result == -2) {
        puts("[Error] Invalid Gyro FSR value");
        return false;
    }

    result = mpu9150_set_accel_fsr(dev, AFSR);
    if (result == -1) {
        puts("[Error] The given i2c is not enabled");
        return false;
    }
    else if (result == -2) {
        puts("[Error] Invalid Accel FSR value");
        return false;
    }

    mpu9150_set_sample_rate(dev, GA_SAMPLE_RATE_HZ);
    uint16_t gaSampleRate = dev->conf.sample_rate;
    printf("G+A sample rate set to: %u (requested rate was %u)\n", gaSampleRate, GA_SAMPLE_RATE_HZ);

    mpu9150_set_compass_sample_rate(dev, C_SAMPLE_RATE_HZ);
    uint16_t cSampleRate = dev->conf.compass_sample_rate;
    printf("Compass sample rate set to: %u (requested rate was %u)\n", cSampleRate, C_SAMPLE_RATE_HZ);
    printf("Initialization successful\n\n");

    t0 = xtimer_now64();

    return true;
}

void imuLoop(void)
{  
}

