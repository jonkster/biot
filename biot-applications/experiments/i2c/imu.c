#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <thread.h>
#include <xtimer.h>

#include "periph/i2c.h"

#include "MPU9150.h"  

#define CHECK_BIT(var,pos) ((var) & (1<<(pos))) != 0

//Ascale = AFS_2G;
//Ascale = AFS_4G;
//Ascale = AFS_8G;
aScale_enum aScale = AFS_8G;
gScale_enum gScale = GFS_1000DPS;

float mRes = 1229/4096.0; // 1229 microTesla full scale
float gRes;
float aRes;
float magCalibration[3];
bool imuReady = false;

void dumpPacket(uint16_t packetSizeBytes, uint8_t *packetData)
{
    printf("dump %d bytes of packet data\n", packetSizeBytes);
    for (uint8_t i = 0; i < packetSizeBytes; i++)
    {
        printf("%-4.2d     |", i);
    }
    puts("");
    for (uint8_t i = 0; i < packetSizeBytes; i++)
    {
        printf("---------+");
    }
    puts("");
    for (uint8_t i = 0; i < packetSizeBytes; i++)
    {
        printf("%-9X|", packetData[i]);
    }
    puts("");
}

bool setGyroDegreesPerSecond(uint8_t i2c_dev, gScale_enum dps)
{
    if (i2c_write_reg(i2c_dev, MPU9150_ADDRESS, GYRO_CONFIG, dps) == 1)
        return true;
    puts("could not set gyro sensitivity");
    return false;
}

bool setLowPassFilter(uint8_t i2c_dev, uint8_t flag)
{
    if (i2c_write_reg(i2c_dev, MPU9150_ADDRESS, CONFIG,  flag) == 1)
        return true;
    puts("could not set low pass filter");
    return false;
}



bool setSamplesPerSecond(uint8_t i2c_dev, uint16_t sRate)
{
    // from manual:
    // Sample Rate = Gyroscope Output Rate / (1 + SMPLRT_DIV)
    // therefore SMPLRT_DIV = ((Gyroscope Output Rate)/(Sample Rate)) - 1

    if (sRate < SAMPLERATE_MIN)
        sRate = SAMPLERATE_MIN;
    else if (sRate > SAMPLERATE_MAX)
        sRate = SAMPLERATE_MAX;

    // get low pass filter setting - if dlpf_cg == 0 then 8kHz gyro rate, otherwise 1kHz.
    uint16_t gRate = 1000;
    char d;
    if (i2c_read_reg(i2c_dev, MPU9150_ADDRESS, CONFIG, &d) == 1)
    {
        d &= 0x07;
        if (d == 0)
            gRate = 8000;
    }
    else
    {
        puts("unable to read config register!");
        return false;
    }
    uint16_t smplrtDiv = (gRate/sRate) - 1;
    if (i2c_write_reg(i2c_dev, MPU9150_ADDRESS, SMPLRT_DIV,  smplrtDiv) == 1)
        return true;
    puts("could not set sample rate");
    return false;
}

bool disableFifo(uint8_t i2c_dev)
{
    if (i2c_write_reg(i2c_dev, MPU9150_ADDRESS, FIFO_EN, 0x00) != 1)    // Disable FIFO
        return false;
    return true;
}


bool setFifoToRecord(uint8_t i2c_dev, uint8_t flags)
{
    if (i2c_write_reg(i2c_dev, MPU9150_ADDRESS, INT_ENABLE, 0x00) != 1) // Disable all interrupts
        return false;
    if (! disableFifo(i2c_dev))
        return false;
    if (i2c_write_reg(i2c_dev, MPU9150_ADDRESS, USER_CTRL, 0x02) != 1)  // Reset I2C master
        return false;
    if (i2c_write_reg(i2c_dev, MPU9150_ADDRESS, USER_CTRL, 0x00) != 1)  // Disable FIFO 
        return false;
    if (i2c_write_reg(i2c_dev, MPU9150_ADDRESS, I2C_MST_DELAY_CTRL, 0x80) != 1) // Enable delay of external sensor data until all data registers have been read
        return false;
    if (i2c_write_reg(i2c_dev, MPU9150_ADDRESS, FIFO_EN, flags) != 1)    // set fifo to record flagged data
        return false;
    if (i2c_write_reg(i2c_dev, MPU9150_ADDRESS, USER_CTRL, 0x40) != 1)   // Enable FIFO
        return false;
    return true;
}


void getGres(void) {
        switch (gScale)
        {
                // Possible gyro scales (and their register bit settings) are:
                // 250 DPS (00), 500 DPS (01), 1000 DPS (10), and 2000 DPS  (11). 
                // calculate DPS/(ADC tick) based on that 2-bit value:
                case GFS_250DPS:
                        gRes = 250.0/32768.0;
                        break;
                case GFS_500DPS:
                        gRes = 500.0/32768.0;
                        break;
                case GFS_1000DPS:
                        gRes = 1000.0/32768.0;
                        break;
                case GFS_2000DPS:
                        gRes = 2000.0/32768.0;
                        break;
        }
}

void getAres(void) {
        switch (aScale)
        {
                // Possible accelerometer scales (and their register bit settings) are:
                // 2 Gs (00), 4 Gs (01), 8 Gs (10), and 16 Gs  (11). 
                // calculate DPS/(ADC tick) based on that 2-bit value:
                case AFS_2G:
                        aRes = 2.0/32768.0;
                        break;
                case AFS_4G:
                        aRes = 4.0/32768.0;
                        break;
                case AFS_8G:
                        aRes = 8.0/32768.0;
                        break;
                case AFS_16G:
                        aRes = 16.0/32768.0;
                        break;
        }
}

uint16_t getPacketSizeBytes(uint8_t i2c_dev)
{
    // read what data is being recorded in fifo and use this to calculate how
    // big each record in the fifo is.
    char d;
    if (i2c_read_reg(i2c_dev, MPU9150_ADDRESS, FIFO_EN, &d) == 1)
    {
        uint16_t bytes = 0;
        if (CHECK_BIT(d, 7))
                bytes += 2;   // temperature data
        if (CHECK_BIT(d, 6))
                bytes += 2;   // gyro x data
        if (CHECK_BIT(d, 5))
                bytes += 2;   // gyro y data
        if (CHECK_BIT(d, 4))
                bytes += 2;   // gyro z data
        if (CHECK_BIT(d, 3))
                bytes += 6;   // accel xyz data
        if (CHECK_BIT(d, 2))
                bytes += 1;   // slv3 data
        if (CHECK_BIT(d, 1))
                bytes += 1;   // slv2 data
        if (CHECK_BIT(d, 0))
                bytes += 1;   // slv0 data
        printf("packet size: %d\n", bytes);
        return bytes;
    }
    else
    {
        puts("cannot read fifo en address!");
        return 0;
    }
}


void delayUsec(uint32_t us)
{
    uint32_t last_wakeup = xtimer_now();
    thread_yield();
    xtimer_usleep_until(&last_wakeup, us);
}

int16_t make16BitSignedValue(uint8_t L, uint8_t H, bool littleEndian)
{
    // Turn the MSB and LSB uint8_t pair into a signed 16-bit value
    if (littleEndian)
        return ((int16_t)H << 8) | L;  
    else
        return ((int16_t)L << 8) | H;  
}

int16_t make16BitValue(uint8_t L, uint8_t H, bool littleEndian)
{
    // Turn the MSB and LSB uint8_t pair into a signed 16-bit value
    if (littleEndian)
        return ((uint16_t)H << 8) | L;  
    else
        return ((uint16_t)L << 8) | H;  
}

uint16_t readFifoByteCount(uint16_t i2c_dev)
{
    char rawData[2];
    if (i2c_read_regs(i2c_dev, MPU9150_ADDRESS, FIFO_COUNTH, rawData, 2) == 2)
    {
        //printf("reading byte count: %d %d\n", rawData[0], rawData[1]);
        uint16_t count = make16BitValue(rawData[0], rawData[1], false);
        return count;
    }
    puts("cannot read byte count!!");
    return 0;
}

bool readFifoRecord(uint8_t i2c_dev, uint8_t packetBytes, char *packet)
{
    if (i2c_read_regs(i2c_dev, MPU9150_ADDRESS, FIFO_R_W, packet, packetBytes) != packetBytes)
        return false;
    return true;
}

bool readRegBlock(uint16_t i2c_dev, uint8_t startRegister, uint8_t bytes, uint8_t *destination, bool verbose)
{
    char *rawData;
    rawData = (char*)malloc(bytes * sizeof(char));

    if (verbose)
        printf("reading %u bytes starting at %u: ", bytes, startRegister);

    if (i2c_read_regs(i2c_dev, MPU9150_ADDRESS, startRegister, rawData, bytes) == bytes)
    {

        for (uint8_t i = 0; i < bytes; i += 2)
        {
            if (verbose)
                printf("%X  ", rawData[i]);
            destination[i] = rawData[i];
        }
        if (verbose)
            puts("");
        free(rawData);
        return true;
    }
    else
    {
        free(rawData);
        puts(" - could not read block");
        return false;
    }

}



void calibrateMPU9150(int i2c_dev, uint8_t address)
{  
    // Accumulate gyro and accelerometer data after device initialization. Calculate the average
    // of the at-rest readings and then loads the resulting offsets into accelerometer and gyro bias registers.
    puts("calibrating - please keep sensor still...");

    // reset device, reset all registers, clear gyro and accelerometer bias registers
    uint8_t res = i2c_write_reg(i2c_dev, MPU9150_ADDRESS, PWR_MGMT_1, 0x80); // Write a one to bit 7 reset bit; toggle reset device
    if (res != 1)
    {
        puts("failed to reset IMU device!");
        return;
    }
    delayUsec(100000);  

    // get stable time source
    // Set clock source to be PLL with x-axis gyroscope reference, bits 2:0 = 001
    res = i2c_write_reg(i2c_dev, MPU9150_ADDRESS, PWR_MGMT_1, 0x01);
    res = i2c_write_reg(i2c_dev, MPU9150_ADDRESS, PWR_MGMT_2, 0x00);
    delayUsec(100000);  

    // Configure device for bias calculation
    res = i2c_write_reg(i2c_dev, MPU9150_ADDRESS, INT_ENABLE, 0x00);   // Disable all interrupts
    res = i2c_write_reg(i2c_dev, MPU9150_ADDRESS, FIFO_EN, 0x00);      // Disable FIFO
    //res = i2c_write_reg(i2c_dev, MPU9150_ADDRESS, PWR_MGMT_1, 0x00);   // Turn on internal clock source
    res = i2c_write_reg(i2c_dev, MPU9150_ADDRESS, I2C_MST_CTRL, 0x00); // Disable I2C master
    res = i2c_write_reg(i2c_dev, MPU9150_ADDRESS, USER_CTRL, 0x00);    // Disable FIFO and I2C master modes
    res = i2c_write_reg(i2c_dev, MPU9150_ADDRESS, USER_CTRL, 0x0C);    // Reset FIFO and DMP
    delayUsec(100000);  
    
    // Configure MPU6050 gyro and accelerometer for bias calculation
    setLowPassFilter(i2c_dev, LPF_180HZ);// Set low-pass filter to 188 Hz
    setSamplesPerSecond(i2c_dev, 1000);  // Set sample rate to 1 kHz
    setGyroDegreesPerSecond(i2c_dev, GFS_250DPS);  // Set gyro full-scale to 250 degrees per second, maximum sensitivity

uint8_t acc[6];    
if (readRegBlock(i2c_dev, ACCEL_XOUT_H, 6, acc, false))
{
    printf("%02X %02X %02X %02X %02X %02X\n", acc[0], acc[1], acc[2], acc[3], acc[4], acc[5]);
}

    // Configure FIFO to capture accelerometer and gyro data for bias calculation
    //res = i2c_write_reg(i2c_dev, MPU9150_ADDRESS, USER_CTRL, 0x40);   // Enable FIFO  
    setFifoToRecord(i2c_dev, FIFO_EN_AXYZ); // Enable accelerometer sensors for FIFO
    //setFifoToRecord(i2c_dev, FIFO_EN_GX | FIFO_EN_GY | FIFO_EN_GZ | FIFO_EN_AXYZ); // Enable gyro and accelerometer sensors for FIFO
    uint16_t packetSizeBytes = getPacketSizeBytes(i2c_dev);

    delayUsec(80000); // accumulate 80 samples in 80 milliseconds = 960 bytes
    // At end of sample accumulation, turn off FIFO sensor read
    disableFifo(i2c_dev);

    uint16_t fifoBytes = readFifoByteCount(i2c_dev);
    char data[packetSizeBytes]; // data array to hold one sample of accelerometer and gyro x, y, z, data
    memset(data, 0, packetSizeBytes);
    uint16_t packetCount = fifoBytes/packetSizeBytes;// How many samples of full gyro and accelerometer data for averaging
    printf("bytes %d packet count %d\n", fifoBytes, packetCount);
    int32_t gyro_bias[3]  = {0, 0, 0};
    int32_t accel_bias[3] = {0, 0, 0};
    printf("reading %d stationary values...\n", packetCount);
    for (uint16_t ii = 0; ii < packetCount; ii++) {
        readFifoRecord(i2c_dev, packetSizeBytes, data);
        dumpPacket(packetSizeBytes, (uint8_t*)&data);
//printf("%2d: %3X %3X %3X %3X %3X %3X %3X %3X %3X %3X %3X %3X    ", ii, data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7], data[8], data[9], data[10], data[11]);
fifoBytes = readFifoByteCount(i2c_dev);
uint16_t pc = fifoBytes/packetSizeBytes;// How many sets of full gyro and accelerometer data for averaging
printf(" now bytes %d packet count %d\n", fifoBytes, pc);
        /*int16_t accel_temp[3] = {0, 0, 0};
        accel_temp[0] = make16BitSignedValue(data[0], data[1], false) ;  // Form signed 16-bit integer for each sample in FIFO
        accel_temp[1] = make16BitSignedValue(data[2], data[3], false) ;
        accel_temp[2] = make16BitSignedValue(data[4], data[5], false) ;

        int16_t gyro_temp[3] = {0, 0, 0};
        gyro_temp[0] = make16BitSignedValue(data[6], data[7], false) ;
        gyro_temp[1] = make16BitSignedValue(data[8], data[9], false) ;
        gyro_temp[2] = make16BitSignedValue(data[10], data[11], false) ;

        accel_bias[0] += (int32_t) accel_temp[0]; // Sum individual signed 16-bit biases to get accumulated signed 32-bit biases
        accel_bias[1] += (int32_t) accel_temp[1];
        accel_bias[2] += (int32_t) accel_temp[2];

        gyro_bias[0]  += (int32_t) gyro_temp[0];
        gyro_bias[1]  += (int32_t) gyro_temp[1];
        gyro_bias[2]  += (int32_t) gyro_temp[2];*/

    }
    
    //Serial.println("read stationary values.");
    accel_bias[0] /= (int32_t) packetCount; // Normalize sums to get average count biases
    accel_bias[1] /= (int32_t) packetCount;
    accel_bias[2] /= (int32_t) packetCount;

    gyro_bias[0]  /= (int32_t) packetCount;
    gyro_bias[1]  /= (int32_t) packetCount;
    gyro_bias[2]  /= (int32_t) packetCount;

    uint16_t  accelsensitivity = 16384;  // = 16384 LSB/g
    if (accel_bias[2] > 0L) 
        accel_bias[2] -= (int32_t) accelsensitivity;  // Remove gravity from the z-axis accelerometer bias calculation
    else 
        accel_bias[2] += (int32_t) accelsensitivity;

    // Construct the gyro biases for push to the hardware gyro bias registers, which are reset to zero upon device startup
    data[0] = (-gyro_bias[0]/4  >> 8) & 0xFF; // Divide by 4 to get 32.9 LSB per deg/s to conform to expected bias input format
    data[1] = (-gyro_bias[0]/4)       & 0xFF; // Biases are additive, so change sign on calculated average gyro biases
    data[2] = (-gyro_bias[1]/4  >> 8) & 0xFF;
    data[3] = (-gyro_bias[1]/4)       & 0xFF;
    data[4] = (-gyro_bias[2]/4  >> 8) & 0xFF;
    data[5] = (-gyro_bias[2]/4)       & 0xFF;

    // Push gyro biases to hardware registers
    res = i2c_write_reg(i2c_dev, MPU9150_ADDRESS, XG_OFFS_USRH, data[0]);
    res = i2c_write_reg(i2c_dev, MPU9150_ADDRESS, XG_OFFS_USRL, data[1]);
    res = i2c_write_reg(i2c_dev, MPU9150_ADDRESS, YG_OFFS_USRH, data[2]);
    res = i2c_write_reg(i2c_dev, MPU9150_ADDRESS, YG_OFFS_USRL, data[3]);
    res = i2c_write_reg(i2c_dev, MPU9150_ADDRESS, ZG_OFFS_USRH, data[4]);
    res = i2c_write_reg(i2c_dev, MPU9150_ADDRESS, ZG_OFFS_USRL, data[5]);

    // Output scaled gyro biases for display in the main program
    //uint16_t  gyrosensitivity  = 131;   // = 131 LSB/degrees/sec
    /*dest1[0] = (float) gyro_bias[0]/(float) gyrosensitivity;  
    dest1[1] = (float) gyro_bias[1]/(float) gyrosensitivity;
    dest1[2] = (float) gyro_bias[2]/(float) gyrosensitivity;*/

    // Construct the accelerometer biases for push to the hardware accelerometer bias registers. These registers contain
    // factory trim values which must be added to the calculated accelerometer biases; on boot up these registers will hold
    // non-zero values. In addition, bit 0 of the lower byte must be preserved since it is used for temperature
    // compensation calculations. Accelerometer bias registers expect bias input as 2048 LSB per g, so that
    // the accelerometer biases calculated above must be divided by 8.

    int32_t accel_bias_reg[3] = {0, 0, 0}; // A place to hold the factory accelerometer trim biases
    i2c_read_regs(i2c_dev, address, XA_OFFSET_H, &data[0], 2); // Read factory accelerometer trim values
    accel_bias_reg[0] = make16BitSignedValue(data[0], data[1], false);
    i2c_read_regs(i2c_dev, address, YA_OFFSET_H, &data[0], 2); // Read factory accelerometer trim values
    accel_bias_reg[1] = make16BitSignedValue(data[0], data[1], false);
    i2c_read_regs(i2c_dev, address, ZA_OFFSET_H, &data[0], 2); // Read factory accelerometer trim values
    accel_bias_reg[2] = make16BitSignedValue(data[0], data[1], false);

    uint32_t mask = 1uL; // Define mask for temperature compensation bit 0 of lower byte of accelerometer bias registers
    uint8_t mask_bit[3] = {0, 0, 0}; // Define array to hold mask bit for each accelerometer bias axis

    for(uint16_t ii = 0; ii < 3; ii++)
    {
        if (accel_bias_reg[ii] & mask)
            mask_bit[ii] = 0x01; // If temperature compensation bit is set, record that fact in mask_bit
    }

    // Construct total accelerometer bias, including calculated average accelerometer bias from above
    accel_bias_reg[0] -= (accel_bias[0]/8); // Subtract calculated averaged accelerometer bias scaled to 2048 LSB/g (16 g full scale)
    accel_bias_reg[1] -= (accel_bias[1]/8);
    accel_bias_reg[2] -= (accel_bias[2]/8);

    data[0] = (accel_bias_reg[0] >> 8) & 0xFF;
    data[1] = (accel_bias_reg[0])      & 0xFF;
    data[1] = data[1] | mask_bit[0]; // preserve temperature compensation bit when writing back to accelerometer bias registers
    data[2] = (accel_bias_reg[1] >> 8) & 0xFF;
    data[3] = (accel_bias_reg[1])      & 0xFF;
    data[3] = data[3] | mask_bit[1]; // preserve temperature compensation bit when writing back to accelerometer bias registers
    data[4] = (accel_bias_reg[2] >> 8) & 0xFF;
    data[5] = (accel_bias_reg[2])      & 0xFF;
    data[5] = data[5] | mask_bit[2]; // preserve temperature compensation bit when writing back to accelerometer bias registers

    // Push accelerometer biases to hardware registers
    res = i2c_write_reg(i2c_dev, MPU9150_ADDRESS, XA_OFFSET_H, data[0]);
    res = i2c_write_reg(i2c_dev, MPU9150_ADDRESS, XA_OFFSET_L_TC, data[1]);
    res = i2c_write_reg(i2c_dev, MPU9150_ADDRESS, YA_OFFSET_H, data[2]);
    res = i2c_write_reg(i2c_dev, MPU9150_ADDRESS, YA_OFFSET_L_TC, data[3]);
    res = i2c_write_reg(i2c_dev, MPU9150_ADDRESS, ZA_OFFSET_H, data[4]);
    res = i2c_write_reg(i2c_dev, MPU9150_ADDRESS, ZA_OFFSET_L_TC, data[5]);

    // Output scaled accelerometer biases for display in the main program
    /*dest2[0] = (float)accel_bias[0]/(float)accelsensitivity; 
    dest2[1] = (float)accel_bias[1]/(float)accelsensitivity;
    dest2[2] = (float)accel_bias[2]/(float)accelsensitivity;*/

    puts("Calibration complete.");

}

void resetFifo(uint16_t i2c_dev)
{
        uint8_t res = i2c_write_reg(i2c_dev, MPU9150_ADDRESS, USER_CTRL, 0x00);  // Disable FIFO 
        if (res == 1)
        {
            res = i2c_write_reg(i2c_dev, MPU9150_ADDRESS, USER_CTRL, 0x44);   // Enable FIFO (and clear it)
            return;
        }
        puts("could not disable FIFO");
}

bool dataReady(uint16_t i2c_dev)
{
    uint8_t d;
    uint16_t res = i2c_read_reg(i2c_dev, MPU9150_ADDRESS, INT_STATUS, (char*)&d);
    if (res != 1)
    {
        return false;
    }
    if (CHECK_BIT(d, 4))
    {
        puts("fifo oflow");
        resetFifo(i2c_dev);
        return false;
    }
    return (CHECK_BIT(d, 0));
}

void initAK8975A(uint16_t i2c_dev, float * destination)
{
        uint16_t res = i2c_write_reg(i2c_dev, AK8975A_ADDRESS, AK8975A_CNTL, POWER_DOWN_MODE); // Power down
        if (res != 1)
        {
            puts("could not power down magnetometer");
            return;
        }
        delayUsec(2000);
        res = i2c_write_reg(i2c_dev, AK8975A_ADDRESS, AK8975A_CNTL, FUSE_ROM_MODE); // Enter Fuse ROM access mode
        delayUsec(2000);

        char rawData[3];  // x/y/z mag register data stored here
        res = i2c_read_regs(i2c_dev, AK8975A_ADDRESS, AK8975A_ASAX, rawData, 3); // read FIFO sample count
        destination[0] =  (float)(rawData[0] - 128)/256.0 + 1.0; // Return x-axis sensitivity adjustment values
        destination[1] =  (float)(rawData[1] - 128)/256.0 + 1.0;  
        destination[2] =  (float)(rawData[2] - 128)/256.0 + 1.0; 

        puts("Magnetometer calibration values: ");
        printf("X-Axis sensitivity adjustment value %f\n", destination[0]);
        printf("Y-Axis sensitivity adjustment value %f\n", destination[1]);
        printf("Z-Axis sensitivity adjustment value %f\n", destination[2]); 

        puts("AK8975A");
        printf("ASAX %d\n", rawData[0]);
        printf("ASAY %d\n", rawData[1]);
        printf("ASAZ %d\n", rawData[2]);
        puts("Magnetometer Setup complete.");
}

void initMPU9150(uint16_t i2c_dev)
{  
    // wake up device
    int res = i2c_write_reg(i2c_dev, MPU9150_ADDRESS, PWR_MGMT_1, 0x00); // Clear sleep mode bit (6), enable all sensors 
    if (res != 1)
    {
        puts("could not wake device!");
        return;
    }
    // get stable time source
    res = i2c_write_reg(i2c_dev, MPU9150_ADDRESS, PWR_MGMT_1, 0x01);  // Set clock source to be PLL with x-axis gyroscope reference, bits 2:0 = 001
    delayUsec(100000); // Delay 100 ms for PLL to get established on x-axis gyro; should check for PLL ready interrupt  

    setLowPassFilter(i2c_dev, LPF_40HZ);
    setSamplesPerSecond(i2c_dev, 100);

    // Set gyroscope full scale range
    // Range selects FS_SEL and AFS_SEL are 0 - 3, so 2-bit values are left-shifted into positions 4:3
    res = i2c_write_reg(i2c_dev, MPU9150_ADDRESS, ACCEL_CONFIG, 0 | gScale << 3);

    // Set accelerometer configuration
    res = i2c_write_reg(i2c_dev, MPU9150_ADDRESS, ACCEL_CONFIG, 0 | aScale << 3);


    // Configure Magnetometer for FIFO
    // Initialize AK8975A for write
    res = i2c_write_reg(i2c_dev, MPU9150_ADDRESS, I2C_SLV1_ADDR, 0x0C);  // Write address of AK8975A
    res = i2c_write_reg(i2c_dev, MPU9150_ADDRESS, I2C_SLV1_REG, 0x0A);   // Register from within the AK8975 to which to write
    res = i2c_write_reg(i2c_dev, MPU9150_ADDRESS, I2C_SLV1_DO, 0x01);    // Register that holds output data written into Slave 1 when in write mode
    res = i2c_write_reg(i2c_dev, MPU9150_ADDRESS, I2C_SLV1_CTRL, 0x81);  // Enable Slave 1

    // Set up auxilliary communication with AK8975A for FIFO read
    res = i2c_write_reg(i2c_dev, MPU9150_ADDRESS, I2C_SLV0_ADDR, 0x8C); // Enable and read address (0x0C) of the AK8975A
    res = i2c_write_reg(i2c_dev, MPU9150_ADDRESS, I2C_SLV0_REG, 0x03);  // Register within AK8975A from which to start data read
    res = i2c_write_reg(i2c_dev, MPU9150_ADDRESS, I2C_SLV0_CTRL, 0x86); // Read six bytes and swap bytes

    // Configure FIFO
    //setFifoToRecord(FIFO_EN_GX | FIFO_EN_GY | FIFO_EN_GZ | FIFO_EN_AXYZ);
    if (! setFifoToRecord(i2c_dev, FIFO_EN_GX))
    {
        puts("could not set fifo up!!");
        return;
    }

    // Configure Interrupts
    // Set interrupt pin active high, push-pull, and clear on read of INT_STATUS
    res = i2c_write_reg(i2c_dev, MPU9150_ADDRESS, INT_PIN_CFG, 0x22);    
    res = i2c_write_reg(i2c_dev, MPU9150_ADDRESS, INT_ENABLE, 0x11); // set interrupt enable on data ready and fifo oflow (bits 4 and 0)

    res = i2c_write_reg(i2c_dev, MPU9150_ADDRESS, SIGNAL_PATH_RESET, 0x7); // reset gyro, accel, temp signal paths
    puts("MPU9150 initialised");
}

bool isCorrectSensor(int i2c_dev, uint8_t address, uint8_t whoAmIRegister, uint8_t expected)
{
    puts("identify IMU");
    uint8_t data;
    uint8_t res = i2c_read_reg(i2c_dev, address, whoAmIRegister, (char*)&data);
    if (res == 1)
    {
        if (data != expected)
        {
            printf("unexpected response: got '%u' (expected '%u')\n", data, expected);  
            return false;
        }
        return true;
    }
    puts("could not detect IMU unit on I2C");
    return false;
}


void imuLoop(uint16_t i2c_dev)
{  
    uint16_t i = 0;
    //uint32_t cTime = 0;

    uint8_t maxBacklog = 40;


    if (imuReady)
    {
        //uint16_t packetSizeBytes = getPacketSizeBytes(i2c_dev);
        uint16_t packetSizeBytes = 1;
if (i == 0)
{
puts("now disable fifo");
disableFifo(i2c_dev);
delayUsec(100000);
}
        while(imuReady)
        {
            uint16_t countBytes = readFifoByteCount(i2c_dev);
            uint16_t count = countBytes/packetSizeBytes;
printf("fifo has %d recs (%d bytes)\n", count, countBytes);
            if (count > 0)
            {
                //int16_t packetData[packetSizeBytes];
                if (countBytes >= 1024)
                {
                    puts("MPU9150 fifo has overflowed");
                    resetFifo(i2c_dev);
                }
                else
                {
                    if (countBytes > maxBacklog * packetSizeBytes)
                    {
                        // we have unacceptable fifo backlog, drop some records
                        /*puts("losing battle");
                        for (uint8_t i = 0; i < 5; i++)
                        {
                            readSensorBlock(i2c_dev, FIFO_R_W, packetSizeBytes, packetData, false);
                        }*/
                    }
                    //readFifoByte(i2c_dev);
                    /*readSensorBlock(i2c_dev, FIFO_R_W, 2*packetSizeBytes, packetData, true);
                    dumpFifoPacket(packetSizeBytes/2, packetData);*/
                    //readSensorBlock(i2c_dev, EXT_SENS_DATA_00, magBlockWords*2, magBlock, false);

                    /*// calculate the accleration value in g's
                    int16_t ax = agBlock[0] * (aRes * 100);
                    int16_t ay = agBlock[1] * (aRes * 100);   
                    int16_t az = agBlock[2] * (aRes * 100);

                    // calculate the gyro value in degrees per second
                    int16_t gx = agBlock[3] * gRes;
                    int16_t gy = agBlock[4] * gRes;  
                    int16_t gz = agBlock[5] * gRes;   

                    int16_t mx = ((float)magBlock[0]*mRes)*(magCalibration[0]);
                    int16_t my = ((float)magBlock[1]*mRes)*(magCalibration[1]);
                    int16_t mz = ((float)magBlock[2]*mRes)*(magCalibration[2]);

                    printf("%5d a(%5d %5d %5d)      g(%5d %5d %5d)      m(%5d %5d %5d)    time(%lu)\n", i++, ax, ay, az, gx, gy, gz, mx, my, mz, xtimer_now() - cTime);*/
                }
            }
            delayUsec(1000);
            if (i++ == 4)
                imuReady = false;
            //cTime = xtimer_now();
        }
    }
    else
    {
        puts("IMU not ready");
    }
}

bool selfTest(int i2c_dev, uint8_t address)
{
        // Show percent deviation from factory trim values, +/- 14 or less deviation is a pass

        // Configure the accelerometer for self-test
        uint8_t res = i2c_write_reg(i2c_dev, MPU9150_ADDRESS, ACCEL_CONFIG, 0xF0); // Enable self test on all three axes and set accelerometer range to +/- 8 g
        if (res != 1)
        {
            puts("failed to read accel config");
            return false;
        }
        res = i2c_write_reg(i2c_dev, MPU9150_ADDRESS, GYRO_CONFIG, 0xE0);  // Enable self test on all three axes and set gyro range to +/- 250 degrees/s
        if (res != 1)
        {
            puts("failed to read gyro config");
            return false;
        }
        //thread_yield();

        char rawData[4];
        res = i2c_read_reg(i2c_dev, address, SELF_TEST_X, rawData);
        res = i2c_read_reg(i2c_dev, address, SELF_TEST_Y, rawData+1);
        res = i2c_read_reg(i2c_dev, address, SELF_TEST_Z, rawData+2);
        res = i2c_read_reg(i2c_dev, address, SELF_TEST_A, rawData+3); // Mixed-axis self-test results

        // Extract the acceleration test results first (NB the registers are smeared hence the different values used to extract the XA/YA/ZA values)
        uint8_t selfTest[6];
        selfTest[0] = (rawData[0] >> 3) | (rawData[3] & 0x30) >> 4 ; // XA_TEST result is a five-bit unsigned integer
        selfTest[1] = (rawData[1] >> 3) | (rawData[3] & 0x0C) >> 4 ; // YA_TEST result is a five-bit unsigned integer
        selfTest[2] = (rawData[2] >> 3) | (rawData[3] & 0x03) >> 4 ; // ZA_TEST result is a five-bit unsigned integer
        // Extract the gyration test results first
        selfTest[3] = rawData[0]  & 0x1F ; // XG_TEST result is a five-bit unsigned integer
        selfTest[4] = rawData[1]  & 0x1F ; // YG_TEST result is a five-bit unsigned integer
        selfTest[5] = rawData[2]  & 0x1F ; // ZG_TEST result is a five-bit unsigned integer   

        // Process results to allow final comparison with factory set values
        float factoryTrim[6];
        factoryTrim[0] = (4096.0*0.34)*(pow( (0.92/0.34) , (((float)selfTest[0] - 1.0)/30.0))); // FT[Xa] factory trim calculation
        factoryTrim[1] = (4096.0*0.34)*(pow( (0.92/0.34) , (((float)selfTest[1] - 1.0)/30.0))); // FT[Ya] factory trim calculation
        factoryTrim[2] = (4096.0*0.34)*(pow( (0.92/0.34) , (((float)selfTest[2] - 1.0)/30.0))); // FT[Za] factory trim calculation
        factoryTrim[3] =  ( 25.0*131.0)*(pow( 1.046 , ((float)selfTest[3] - 1.0) ));            // FT[Xg] factory trim calculation
        factoryTrim[4] =  (-25.0*131.0)*(pow( 1.046 , ((float)selfTest[4] - 1.0) ));            // FT[Yg] factory trim calculation
        factoryTrim[5] =  ( 25.0*131.0)*(pow( 1.046 , ((float)selfTest[5] - 1.0) ));            // FT[Zg] factory trim calculation

        // To get to percent, must multiply by 100 and subtract result from 100
        float selfTestf[6];
        for (int i = 0; i < 6; i++) {
                selfTestf[i] = 100.0 + 100.0*((float)selfTest[i] - factoryTrim[i])/factoryTrim[i]; // Report percent differences
        }

        printf("x-axis self test: acceleration trim within : %f%% of factory value\n", selfTestf[0]);
        printf("y-axis self test: acceleration trim within : %f%% of factory value\n", selfTestf[1]);
        printf("z-axis self test: acceleration trim within : %f%% of factory value\n", selfTestf[2]);
        printf("x-axis self test: gyration trim within : %f%% of factory value\n", selfTestf[3]);
        printf("y-axis self test: gyration trim within : %f%% of factory value\n", selfTestf[4]);
        printf("z-axis self test: gyration trim within : %f%% of factory value\n", selfTestf[5]);

        if(selfTestf[0] < 1.0f && selfTestf[1] < 1.0f && selfTestf[2] < 1.0f && selfTestf[3] < 1.0f && selfTestf[4] < 1.0f && selfTestf[5] < 1.0f) {
                puts("Pass Selftest!");  
                return true;
        }
        else
        {
                puts("MPU9150 is out of spec!");
                return false;
        }
}

bool setupMPU9150(uint16_t i2c_dev)
{
    if (isCorrectSensor(i2c_dev, MPU9150_ADDRESS, WHO_AM_I_MPU9150, MPU9150_ADDRESS))
    {
        puts("MPU9150 device detected...");
        if (selfTest(i2c_dev, MPU9150_ADDRESS))
        {
            puts("MPU9150 passes self test.  Calibrating...");
            calibrateMPU9150(i2c_dev, MPU9150_ADDRESS); // Calibrate gyro and accelerometers, load biases in bias registers  
            initMPU9150(i2c_dev);
            getGres();
            getAres();

            if (isCorrectSensor(i2c_dev, AK8975A_ADDRESS, WHO_AM_I_AK8975A, AK8975A_WHO_RESPONSE))
            {
                // Get magnetometer calibration from AK8975A ROM
                puts("Init AK8975A....");
                initAK8975A(i2c_dev, magCalibration);
            }
            else
            {
                puts("could not initialise magnetometer");
                return 1;
            }
            imuReady = true;
            return 0;
        }
        else
        {
            puts("fails self test.");
            return 1;
        }
    }
    else
    {
        puts("No IMU device detected on I2C port.");
        return 1;
    }
}
