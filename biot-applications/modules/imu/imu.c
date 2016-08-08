#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <thread.h>
#include <xtimer.h>


#include "imu.h"
#include "../identify/biotIdentify.h"

#define CHECK_BIT(var,pos) ((var) & (1<<(pos))) != 0

uint64_t t0;
myQuat_t currentQ;
imuData_t lastIMUData;

int16_t magMinMax[6] = { 0, 0, 0, 0, 0, 0 };

int16_t magHardCorrection[3] = { 0, 0, 0 };
double magSoftCorrection[3] = { 1, 1, 1 };
uint16_t failureCount = 0;
uint16_t magValid = 0;


bool useAccelerometers = true;
bool useMagnetometers = true;
bool useGyroscopes = true;


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
            magMinMax[X_AXIS], magMinMax[Y_AXIS], magMinMax[Z_AXIS],
            magMinMax[3], magMinMax[4], magMinMax[5],
            magHardCorrection[X_AXIS], magHardCorrection[Y_AXIS], magHardCorrection[Z_AXIS],
            magSoftCorrection[X_AXIS], magSoftCorrection[Y_AXIS], magSoftCorrection[Z_AXIS]);
    printf("+-------------------------------------+\n");
}


void displayData(imuData_t data)
{
    printf( "%"PRIu32", a("
            "%"PRId16", %"PRId16", %"PRId16"), g("
            "%"PRId16", %"PRId16", %"PRId16"), m( "
            "%"PRId16", %"PRId16", %"PRId16"), t"
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

void forceReorientation(void)
{
    magValid = 0;
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

int16_t *getMagCalibration(void)
{
    return magMinMax;
}



void imuCalibrate(imuData_t *data)
{
    // find minimums
    if (data->mag.x_axis < magMinMax[X_AXIS])
        magMinMax[X_AXIS] = data->mag.x_axis;

    if (data->mag.y_axis < magMinMax[Y_AXIS])
        magMinMax[Y_AXIS] = data->mag.y_axis;

    if (data->mag.z_axis < magMinMax[Z_AXIS])
        magMinMax[Z_AXIS] = data->mag.z_axis;

    // find maximums
    if (data->mag.x_axis > magMinMax[3])
        magMinMax[3] = data->mag.x_axis;

    if (data->mag.y_axis > magMinMax[4])
        magMinMax[4] = data->mag.y_axis;

    if (data->mag.z_axis > magMinMax[5])
        magMinMax[5] = data->mag.z_axis;


    setMagCalibration(magMinMax);
}

/*myQuat_t rollPitchYawToQ(double *ypr)
{
    double c1 = cos(ypr[Z_AXIS]);
    double s1 = sin(ypr[Z_AXIS]);
    double c2 = cos(ypr[Y_AXIS]);
    double s2 = sin(ypr[Y_AXIS]);
    double c3 = cos(ypr[X_AXIS]);
    double s3 = sin(ypr[X_AXIS]);
    myQuat_t q;

    q.w = sqrt(1.0 + c1 * c2 + c1*c3 - s1 * s2 * s3 + c2*c3) / 2.0;
    double w4 = (4.0 * q.w);
    q.x = (c2 * s3 + c1 * s3 + s1 * s2 * c3) / w4 ;
    q.y = (s1 * c2 + s1 * c3 + c1 * s2 * s3) / w4 ;
    q.z = (-s1 * s3 + c1 * s2 * c3 +s2) / w4 ;
    return q;
}*/


/*myQuat_t magToQuat(double *mag)
{
    double worldNorth[3] = { 1.0, 0, 0 };
    double angle = acos(vecDot(mag, worldNorth));

    double vec[3];
    vecCross(vec, mag, worldNorth);
    vecNormalise(vec);

    return quatAngleAxis(angle, vec);
}*/

bool oppositeSign(double a, double b)
{
    return (((a > 0) && (b < 0)) || ((a < 0) && (b > 0)));
}

myQuat_t adjustForCongruence(myQuat_t measuredQ, myQuat_t deducedQ)
{
    // find largest component of quaternion
    double max = -1000;
    double measuredQComponent = measuredQ.w;
    double deducedQComponent = deducedQ.w;
    if (fabs(measuredQ.w) > max)
    {
        max = fabs(measuredQ.w);
    }
    if (fabs(measuredQ.x) > max)
    {
        max = fabs(measuredQ.x);
        measuredQComponent = measuredQ.x;
        deducedQComponent = deducedQ.x;
    }
    if (fabs(measuredQ.y) > max)
    {
        max = fabs(measuredQ.y);
        measuredQComponent = measuredQ.y;
        deducedQComponent = deducedQ.y;
    }
    if (fabs(measuredQ.z) > max)
    {
        max = fabs(measuredQ.z);
        measuredQComponent = measuredQ.z;
        deducedQComponent = deducedQ.z;
    }

    if (oppositeSign(measuredQComponent, deducedQComponent))
    {
        measuredQ = quatScalarMultiply(measuredQ, -1);
    }
    /*double dq = quatDiffMagnitude(measuredQ, deducedQ);
    if (dq > PI/6.0)
    {
        printf("%6.2f > %6.2f\n", dq, PI/4.0);
        if (magValid < 20)
            magValid++;
        else
            measuredQ = fallBackQ;
    }*/
    return measuredQ;
}

myQuat_t getPosition(mpu9150_t dev)
{
    imuData_t imuData;
    if (getIMUData(dev, &imuData))
    {
        if (! validIMUData(imuData))
        {
            if (failureCount++ > 20)
            {
                puts("Error in IMU, restarting!\n");
                identifyYourself("imu failure");
                if (! initialiseIMU(&dev))
                {
                    puts("Could not initialise IMU!\n");
                    exit(1);
                }
            }
            return currentQ;
        }
        failureCount = 0;
        // get orientation as deduced by adding gyro measured rotational
        // velocity (times time interval) to previous orientation.
        double gx1 = (double)(imuData.gyro.x_axis);
        double gy1 = (double)(imuData.gyro.y_axis);
        double gz1 = (double)(imuData.gyro.z_axis);
        double omega[3] = { gx1, gy1, gz1 }; // this will be in degrees/sec

        uint32_t dt = imuData.ts - lastIMUData.ts; // dt in microseconds
        myQuat_t gRot = makeQuatFromAngularVelocityTime(omega, dt/1000000.0);
        myQuat_t gyroDeducedQ =  quatMultiply(currentQ, gRot);
        quatNormalise(&gyroDeducedQ);
        if (! useGyroscopes)
        {
            makeIdentityQuat(&gyroDeducedQ);
        }

        // get orientation from the gyro as Roll Pitch Yaw (ie Euler angles)
        double gyroDeducedYPR[3];
        quatToEuler(gyroDeducedQ, gyroDeducedYPR);

        // get gravity direction from accelerometer
        double ax1 = imuData.accel.x_axis / 1024.0;
        double ay1 = imuData.accel.y_axis / 1024.0;
        double az1 = imuData.accel.z_axis / 1024.0;
        double downSensor[3] = { ax1, ay1, az1 };
        if (fabs(vecLength(downSensor) - 1) > 0.2)
        {
            // excessive accelerometer reading, bail out
            currentQ = gyroDeducedQ;
            lastIMUData = imuData;
            return currentQ;
        }
        vecNormalise(downSensor);

        // Calculate a roll/pich/yaw from the accelerometers and magnetometers.
     
    
        // The accelerometers can only measure pitch and roll (in the world reference
        // frame), calculate the pitch and roll values to account for
        // accelerometer readings, make yaw = 0.
        double ypr[3];
        ypr[ROLL] = atan2(downSensor[Y_AXIS], downSensor[Z_AXIS]);
        ypr[PITCH] = -atan2(downSensor[X_AXIS], sqrt(downSensor[Y_AXIS]*downSensor[Y_AXIS] + downSensor[Z_AXIS]*downSensor[Z_AXIS]));
        ypr[YAW] = 0;  // yaw == 0, accelerometer cannot measure yaw
        if (! useAccelerometers) 
        {
            // if no accelerometers, use roll and pitch values from gyroscope
            // derived orientation
            ypr[PITCH] = gyroDeducedYPR[PITCH];
            ypr[ROLL] = gyroDeducedYPR[ROLL];
            ypr[YAW] = 0;
        }
        myQuat_t accelQ = eulerToQuat(ypr);
        myQuat_t invAccelQ = quatConjugate(accelQ);


        // get magnetometer indication of North
        double mx1 = (double)(imuData.mag.x_axis - magHardCorrection[X_AXIS]);
        double my1 = (double)(imuData.mag.y_axis - magHardCorrection[Y_AXIS]);
        double mz1 = (double)(imuData.mag.z_axis - magHardCorrection[Z_AXIS]);
        mx1 *= magSoftCorrection[X_AXIS];
        my1 *= magSoftCorrection[Y_AXIS];
        mz1 *= magSoftCorrection[Z_AXIS];
        // NB MPU9150 has different XYZ axis for magnetometers than for gyros
        // and accelerometers so juggle x,y,z here...
        double magV[3] = { my1, mx1, -mz1 };
        vecNormalise(magV);
        // make quaternion representing mag readings
        myQuat_t magQ = quatFromValues(0, magV[X_AXIS], magV[Y_AXIS], magV[Z_AXIS]);

        // the magnetometers can only measure yaw (in the world reference
        // frame), adjust the yaw of the roll/pitch/yaw values calculated
        // earlier to account for the magnetometer readings.
        if (! useMagnetometers) 
        {
            ypr[YAW] = gyroDeducedYPR[Z_AXIS];
        }
        else
        {
            // pitch and roll the mag direction using accelerometer info to
            // create a quaternion that represents the IMU as if it was
            // horizontal (so we can get pure yaw)
            myQuat_t horizQ = quatMultiply(accelQ, magQ);
            horizQ = quatMultiply(horizQ, invAccelQ);

            // calculate pure yaw
            ypr[YAW] = fabs(atan2(horizQ.y, horizQ.x));
        }

        // measuredQ is the orientation as measured using the
        // accelerometer/magnetometer readings
        myQuat_t measuredQ = eulerToQuat(ypr);
        // check measured and deduced orientations are not wildly different (eg
        // due to a 360d alias flip) and adjust if problem...
        measuredQ = adjustForCongruence(measuredQ, gyroDeducedQ);

        if (useGyroscopes)
        {
            // the new orientation is the gyro deduced position corrected
            // towards the accel/mag measured position
            currentQ = slerp(gyroDeducedQ, measuredQ, 0.2);
        }
        else
        {
            currentQ = measuredQ;
        }
    }
    if (! isQuatValid(currentQ))
    {
        puts("error in quaternion, reseting orientation...");
        makeIdentityQuat(&currentQ);
    }
    lastIMUData = imuData;
    return currentQ;
}

void initialisePosition(mpu9150_t *dev)
{
    makeIdentityQuat(&currentQ);
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

void setMagCalibration(int16_t *cal)
{
    magMinMax[X_AXIS] = cal[X_AXIS];
    magMinMax[Y_AXIS] = cal[Y_AXIS];
    magMinMax[Z_AXIS] = cal[Z_AXIS];
    magMinMax[3] = cal[3];
    magMinMax[4] = cal[4];
    magMinMax[5] = cal[5];

    // Hard corrections - try and make all mag readings be equally spread
    // across origin (rather than being offset).  Collect max and min values
    // and calculate an 'average centre'.

    for (uint8_t i = 0; i < 3; i++)
        magHardCorrection[i] = (magMinMax[i] + magMinMax[i+3])/2;

    // Soft corrections - try and reduce elongations along x,y,z
    // axis to make the data fit closer to a sphere (this is a bit of a quick and
    // dirty method but hopefully will be OK).

    // take hard corrected data (which should be centred on origin)
    // and along each axis, work out a max/min value
    double vMax[3];
    double vMin[3];
    for (uint8_t i = 0; i < 3; i++)
    {
        vMax[i] = (double)magMinMax[i+3] - (magMinMax[i] + magMinMax[i+3])/2.0;
        vMin[i] = (double)magMinMax[i] - (magMinMax[i] + magMinMax[i])/2.0;
    }
    // find 'average' distance from centre of the 3 axes
    double avg[3];
    for (uint8_t i = 0; i < 3; i++)
    {
        avg[i] = (vMax[i] - vMin[i])/2;
    }
    double averageRadius = (avg[X_AXIS] + avg[Y_AXIS] + avg[Z_AXIS]) / 3;

    // calculate scale factor along each axis that would squash the readings
    // into something closer to a sphere
    for (uint8_t i = 0; i < 3; i++)
    {
        magSoftCorrection[i] = averageRadius/avg[i];
    }
}

bool getAccelUse(void)
{
    return useAccelerometers;
}

bool getCompassUse(void)
{
    return useMagnetometers;
}

bool getGyroUse(void)
{
    return useGyroscopes;
}

void setAccelUse(bool onoff)
{
    useAccelerometers = onoff;
}

void setCompassUse(bool onoff)
{
    useMagnetometers = onoff;
}

void setGyroUse(bool onoff)
{
    useGyroscopes = onoff;
}

bool validIMUData(imuData_t imuData)
{
    if ((imuData.accel.x_axis == 0) && (imuData.accel.y_axis == 0) && (imuData.accel.z_axis == 0))
    {
        return false;
    }
    if ((imuData.mag.x_axis == 0) && (imuData.mag.y_axis == 0) && (imuData.mag.z_axis == 0))
    {
        return false;
    }
    if ((imuData.gyro.x_axis == 0) && (imuData.gyro.y_axis == 0) && (imuData.gyro.z_axis == 0))
    {
        return false;
    }
    return true;
}


