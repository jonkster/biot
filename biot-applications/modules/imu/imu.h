#ifndef _IMU_H_
#define _IMU_H_

#include <stdbool.h>

#include "mpu9250.h"

#include "../position/position.h"

#define GA_SAMPLE_RATE_HZ   400 // must be between 4 and 1000.  200 works
#define C_SAMPLE_RATE_HZ     80 // should be between 1 and 100Hz and an integral fraction of compass/gyro rate, 25 works

#define GFSR                MPU9250_GYRO_FSR_250DPS
#define AFSR                MPU9250_ACCEL_FSR_2G

#define UPDATE_INTERVAL_MS 25

typedef struct {
    uint32_t ts;
    mpu9250_results_t accel;
    mpu9250_results_t gyro;
    mpu9250_results_t mag;
    double temperature;
} imuData_t;

typedef struct {
    bool useGyroscopes;
    bool useAccelerometers;
    bool useMagnetometers;
    uint32_t dupInterval;
    bool calibrateMode;
} imuStatus_t;



#ifdef __cplusplus
extern "C" {
#endif

    extern bool autoCalibrate;
    extern uint32_t dupInterval;

    uint16_t aFsrRange2Int(mpu9250_accel_ranges_t fsr);

    void displayConfiguration(mpu9250_t dev);

    void displayCorrections(void);

    void displayData(imuData_t data);

    void forceReorientation(void);

    uint16_t gFsrRange2Int(mpu9250_gyro_ranges_t fsr);

    bool getIMUData(mpu9250_t dev, imuData_t *data);

    bool getIMUStatus(mpu9250_t dev, imuStatus_t *status);

    void imuCalibrate(imuData_t *data);

    void initialisePosition(void);

    int16_t *getMagCalibration(void);

    myQuat_t getPosition(mpu9250_t dev);

    bool initialiseIMU(mpu9250_t *dev);

    void setMagCalibration(int16_t *cal);

    bool getAccelUse(void);

    bool getCompassUse(void);

    bool getGyroUse(void);

    void setAccelUse(bool onoff);

    void setCompassUse(bool onoff);

    void setGyroUse(bool onoff);

    bool validIMUData(imuData_t imuData);

#ifdef __cplusplus
}
#endif

#endif
