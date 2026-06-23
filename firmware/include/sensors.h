#ifndef SENSORS
#define SENSORS 1

#include "pi3a_core.h"

#ifndef MPU6050_ADDR
#define MPU6050_ADDR 0x68
#endif

// Registradores do MPU-6050
#define GYRO_CONFIG 0x1B
#define ACCEL_CONFIG 0x1C
#define ACCEL_XOUT_H 0x3B
#define ACCEL_YOUT_H 0x3D
#define ACCEL_ZOUT_H 0x3F
#define GYRO_XOUT_H 0x43
#define GYRO_YOUT_H 0x45
#define GYRO_ZOUT_H 0x47
#define PWR_MGMT_1 0x6B

// Configurações do MPU-6050
#define AFS_SEL 0x18         // +/- 16g
#define AFS_SENSITIVITY 2048 // BIT Range / Full Scale Range
#define FS_SEL 0x18          // +/- 2000°/s
#define FS_SENSITIVITY 16.4f // BIT Range / Full Scale Range

// HC-SR04
int hc_sr04(gpio_t trigger, gpio_t echo);

// HW-123E, módulo do MPU-6050
void mpu6050_write(uint8_t reg, uint8_t data);
void mpu6050_init(void);
void mpu6050_read_all(float *ax, float *ay, float *az, float *gx, float *gy, float *gz);
float mpu6050_read_axis(uint8_t reg, float sensitivity);
uint8_t mpu6050_read_reg(uint8_t reg);

#endif