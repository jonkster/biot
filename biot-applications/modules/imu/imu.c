#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <thread.h>
#include <xtimer.h>


#include "imu.h"

#define CHECK_BIT(var,pos) ((var) & (1<<(pos))) != 0

uint64_t t0;
myQuat_t currentIMUPosition;
imuData_t lastIMUData;

int16_t magMin[3] = { 0, 0, 0 };
int16_t magMax[3] = { 0, 0, 0 };
int16_t magHardCorrection[3] = { 0, 0, 0 };
double magSoftCorrection[3] = { 1, 1, 1 };
bool magValid = false;


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

void displayCorrections(void)
{
    printf("+------------Correction Data----------+\n");
    printf(
            "mag min: %"PRId16", %"PRId16", %"PRId16"\n"
            "mag max: %"PRId16", %"PRId16", %"PRId16"\n"
            "hard correction offsets: %"PRId16", %"PRId16", %"PRId16"\n"
            "soft correction factors: %f, %f, %f\n",
            magMin[0], magMin[1], magMin[2],
            magMax[0], magMax[1], magMax[2],
            magHardCorrection[0], magHardCorrection[1], magHardCorrection[2],
            magSoftCorrection[0], magSoftCorrection[1], magSoftCorrection[2]);
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
    imuCalibrate(data);
    int32_t rawTemp;
    mpu9150_read_temperature(&dev, &rawTemp);
    data->temperature = rawTemp/1000; // approx temperature in degrees C

    return true;
}

void imuCalibrate(imuData_t *data)
{
    // hard corrections - try and make all mag readings be equally spread
    // acrooss origin rather than being offset by collecting max and min values
    // and calculating an 'average centre'.
    
    if (data->mag.x_axis > magMax[0])
        magMax[0] = data->mag.x_axis;

    if (data->mag.y_axis > magMax[1])
        magMax[1] = data->mag.y_axis;

    if (data->mag.z_axis > magMax[2])
        magMax[2] = data->mag.z_axis;

    if (data->mag.x_axis < magMin[0])
        magMin[0] = data->mag.x_axis;

    if (data->mag.y_axis < magMin[1])
        magMin[1] = data->mag.y_axis;

    if (data->mag.z_axis < magMin[2])
        magMin[2] = data->mag.z_axis;
    
    for (uint8_t i = 0; i < 3; i++)
        magHardCorrection[i] = (magMin[i] + magMax[i])/2;

    // soft corrections, try and reduce elongations along x,y,z
    // axis to make the data fit closer to a sphere (this a bit of a quick and
    // dirty method but hopefully will be OK).

    // along each axis, work out a max/min value
    double vMax[3];
    double vMin[3];
    for (uint8_t i = 0; i < 3; i++)
    {
        vMax[i] = (double)magMax[i] - (magMin[i] + magMax[i])/2.0;
        vMin[i] = (double)magMin[i] - (magMin[i] + magMin[i])/2.0;
    }
    // find 'average' distance from centre of the 3 axes
    double avg[3];
    for (uint8_t i = 0; i < 3; i++)
    {
        avg[i] = (vMax[i] - vMin[i])/2;
    }
    double averageRadius = (avg[0] + avg[1] + avg[2]) / 3;

    // calculate scale factor along each axis
    for (uint8_t i = 0; i < 3; i++)
    {
        magSoftCorrection[i] = averageRadius/avg[i];
    }
}

myQuat_t getPosition(mpu9150_t dev)
{
    bool agm[3] = {false, false, true};
    imuData_t imuData;
    if (getIMUData(dev, &imuData))
    {
        myQuat_t aRot = newQuat();
        // accelerometer
        if (agm[0])
        {
            double ax1 = (double)imuData.accel.x_axis/1024;
            double ay1 = (double)imuData.accel.y_axis/1024;
            double az1 = (double)imuData.accel.z_axis/1024;

            double ato[3] = { ax1, ay1, az1 };
            if (vecLength(ato) > 0)
            {
                double afrom[3] = {0, 0, 1};
                vecNormalise(ato);
                aRot = quatFrom2Vecs(afrom, ato);
            }
        }

        myQuat_t mRot = newQuat();
        // magnetometer
        if (agm[2])
        {
            double mx1 = (double)(imuData.mag.x_axis - magHardCorrection[0]);
            double my1 = (double)(imuData.mag.y_axis - magHardCorrection[1]);
            double mz1 = (double)(imuData.mag.z_axis - magHardCorrection[2]);
            mx1 *= magSoftCorrection[0];
            my1 *= magSoftCorrection[1];
            mz1 *= magSoftCorrection[2];

            double mto[3] = { mx1, my1, mz1 };
            if (vecLength(mto) > 0)
            {
                /* lets say always constant declination for time being - need
                 * to make this configurable somehow for different locations.
                 */
                double declination = 60.0 * PI / 180.0;

                double mfrom[3] = {cos(declination), sin(declination), 0};
                vecNormalise(mto);
                mRot = quatFrom2Vecs(mfrom, mto);
            }
        }

        myQuat_t iRot = newQuat();
        iRot = quatMultiply(iRot, aRot);
        iRot = quatMultiply(iRot, mRot);
        currentIMUPosition = iRot;

        // gyroscope
        if (agm[1])
        {
            double gx1 = (double)imuData.gyro.x_axis/1024;
            double gy1 = (double)imuData.gyro.y_axis/1024;
            double gz1 = (double)imuData.gyro.z_axis/1024;

            uint32_t dt = imuData.ts - lastIMUData.ts;
            double omega[3] = { gx1, gy1, gz1 }; 
            myQuat_t gRot = makeQuatFromAngularVelocityTime(omega, dt/50000);
            currentIMUPosition = quatMultiply(currentIMUPosition, gRot);
        }

    }
    lastIMUData = imuData;
    return currentIMUPosition;
}

void initialisePosition(mpu9150_t *dev)
{
    makeIdentityQuat(&currentIMUPosition);

    currentIMUPosition = getPosition(*dev);
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

    initialisePosition(dev);
    return true;
}

void imuLoop(void)
{  
}

