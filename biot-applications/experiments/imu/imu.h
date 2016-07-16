#ifndef _IMU_H_
#define _IMU_H_

#include <stdbool.h>

#define GA_SAMPLE_RATE_HZ   300 // must be between 4 and 1000.  200 works
#define C_SAMPLE_RATE_HZ     25 // should be between 1 and 100Hz and an integral fraction of comapss/gyro rate

#define GFSR                MPU9150_GYRO_FSR_2000DPS
#define AFSR                MPU9150_ACCEL_FSR_2G

typedef struct {
    uint32_t ts;
    mpu9150_results_t accel;
    mpu9150_results_t gyro;
    mpu9150_results_t mag;
    float temperature;
} imuData_t;

#ifdef __cplusplus
extern "C" {
#endif

uint16_t aFsrRange2Int(mpu9150_accel_ranges_t fsr);

void displayConfiguration(mpu9150_t dev);

void displayData(imuData_t data);

uint16_t gFsrRange2Int(mpu9150_gyro_ranges_t fsr);

bool getIMUData(mpu9150_t dev, imuData_t *data);

bool initialiseIMU(mpu9150_t *dev);

void imuLoop(void);

#ifdef __cplusplus
}
#endif

#endif
