#ifndef _IMU_H_
#define _IMU_H_

#include <stdbool.h>

#include "mpu9150.h"

#include "../position/position.h"

#define GA_SAMPLE_RATE_HZ   200 // must be between 4 and 1000.  200 works
#define C_SAMPLE_RATE_HZ     25 // should be between 1 and 100Hz and an integral fraction of compass/gyro rate

#define GFSR                MPU9150_GYRO_FSR_2000DPS
#define AFSR                MPU9150_ACCEL_FSR_4G

typedef struct {
    uint32_t ts;
    mpu9150_results_t accel;
    mpu9150_results_t gyro;
    mpu9150_results_t mag;
    double temperature;
} imuData_t;


#ifdef __cplusplus
extern "C" {
#endif

    uint16_t aFsrRange2Int(mpu9150_accel_ranges_t fsr);

    void displayConfiguration(mpu9150_t dev);

    void displayCorrections(void);

    void displayData(imuData_t data);

    uint16_t gFsrRange2Int(mpu9150_gyro_ranges_t fsr);

    bool getIMUData(mpu9150_t dev, imuData_t *data);

    void imuCalibrate(imuData_t *data);

    int16_t *getMagCalibration(void);

    myQuat_t getPosition(mpu9150_t dev);

    bool initialiseIMU(mpu9150_t *dev);

    void setMagCalibration(int16_t *cal);

    bool getAccelUse(void);

    bool getCompassUse(void);

    bool getGyroUse(void);

    void setAccelUse(bool onoff);

    void setCompassUse(bool onoff);

    void setGyroUse(bool onoff);

#ifdef __cplusplus
}
#endif

#endif
